journal.h: struct dhara_journal, epoch, increments whenever journal head passes the end of chip
and wraps around. So the "perfect wear leveling, only off by 1" is achieved via going through
the memory and using always a new page for writing?

log2_ppc: it's the log2 of ppc (pages per checkpoint) of the number of contiguous aligned pages in one "checkpoint" -- a unit in the journal user data is grouped into

journal.c: dhara_journal_init: choosing ppc based on log2 of page size and log2 of ppb (pages per  erase block). choose_ppc "calculate checkpoint period"

choose_ppc(log2_page_sie, max) ~ (log2_page_size, log2_ppb)
! journal.c: choose_ppc(): ppc as a maximum number of metadata "blocks" that can fit (with header&cokie) in one page

trace_path() somehow reconstructs something from the journal? but is the journal
physically stored? in what form? how is it reconstructed on start-up?

on a higher lvl, if it goes in a circular motion, what are the pros and cons of that approach?

!! map_internals.txt !!

Mapping based on binary radix tree. Cannot modify existing objects (flash!) so allocating new LEAF and full path to root, including new ROOT and pointing at older but still valid structures. 4 bit addresses have ROOT + 4 levels and you need to always construct new leaf and full path including root.

Each modification requires `1 + |number of address bits|` (leaf + path up to including root) nodes == pack these into a fixed sized allocation unit.

Alt pointers are pointers downwards from root not on the path to the new sector (those can be inferred).


### pros:
- "perfect wear leveling" (max diff by 1 erase count)
- journal is assumed to be robust. any way to confirm/check? smt like partial verification? out of scope most likely?
- some ECC software implementation for potential strengthening the on-chip ECC

TODO: classify common on device NAND flash ECC strength (relevant bit error rates, uncorrectable, standard requirements etc.)

Dhara has 1-4 bits repair BCH codes, the strongest uses 7 ECC bytes for given chunk (assuming it's used for a page). Meaning the larger a page, the weaker it gets?

### AS5F31G04SND-08LIN
- 8bit ECC for each 512bytes + 32bytes, 4bit ECC for each 512bytes + 16bytes -- that's correction ability, right?
- for 2048 + 64 B pages: 2048B is 4 data storage areas, then 4 * 4B of spare area protected by ECC, then 4*4B spare area not protected, then finally 32B is ECC
- so 32B of ECC is over 2048 + 16 = 2064B, then code ratio is % of useful data (that's 2064B) out of chunk including ECC (2064 + 32 = 2096) so 2064/2096 = 0.984733 ?
- 16 B of ECC protected spare OOB area could be used for additional Dhara ECC? the strongest currently offered uses 7 bytes for ECC, could be extended when proper polynomial parameters are researched?
- would that add anything if the bit-correction-rate would be <= than internal ECC?


TODO: real HW benchmark? and/or over abstracted block device? how to classify performance?
- prolly need to do add verbose "debug prints" to Dhara/dhara_glue/spi_nand_flash and test on HW to capture the flow easier than going through the code
- started ^^ via some guy's example forked of Dhara, using mock NAND

### cons?:
- radix tree queue & dequeue journal, alt-pointers, garbage collection, repacking == quite intricate implementation when compared to legacy (*A) -- see below
- if each page write "emits" a journal update, is the cpu time overhead alright?
- no read disturbance mitigation (fact check?) -- repeated read operations without refreshing the data via writing degrades it -- THIS IS TAKEN CARE OF INTERNALLY IN spi_nand_flash
- ^^ but the same page is rewritten: if it's worn out and becoming faulty, it would make sense to move the data to more healthy page! move the info up to Dhara layer and perform "page" relief?

(*A) comparing to "legacy" WL https://github.com/espressif/esp-idf/tree/master/components/wear_levelling
- legacy on NOR flash, 4KB sector size vs. NAND where each page is 2048/4096 + some OOB
- legacy had multiple counters (incrementing at diferent rates based on erase operations performed) + a simple calculation for logical->physical sector to access in the given address space
- legacy did write to flash in minimal chunks of 16 B ("pos update record") !!! NAND can only write a full page, potentially partial-page programming (need to follow datasheet instructions so on-chip ECC works correctly) meaning an absolutely different approach is necessary
- so Dhara does only full page writes, correct? which would be the easiest approach, not fussing over partial page writes
- only 4 partial page programs allowed on a single page
- NOP == "number of partial programs"

## Improving Dhara: the easiest approach?
- given the journal is a thing we want to keep for robustness, extending Dhara with some level of "unleveling"/page relief based on bit corrections performed on read seems a good approach
- so this would entail keeping some metadata on individual pages and being able to transparently skip a page for writing.
- as Dhara currently does not use any OOB areas, we could easily use them for this
- TODO double check current OOB usage: only bad block marking in first page of a block, first 2 bytes of OOB being non-0xffff (https://github.com/espressif/idf-extra-components/blob/master/spi_nand_flash/src/nand_impl.c#L95)? nothing besides this?
- https://github.com/omnitex/dhara/blob/master/dhara/nand.h#L92 read will have to provide ECC info regardless of any error occurring -- we need the number of bits corrected (can be obtained, if the NAND chips supports it, from ECC portion of status register after a read)
### OBSOLETE START
- ECC correction in status register IS VENDOR SPECIFIC, and can offer different level of detail (Micron 3bit info, Alliance 2bits)
- assuming 2bit info is the universal minimum -- thus we working with 00: no bit errors detected, 01: detected & corrected, 10: uncorrectable detected, 11: max number (8) detected and corrected
- 2bit vs 3bit are "backwards" compatible via 00, 01 and 10 (~same meaning). 11 on 3bit becomes non-max bits corrected but more than 01, with only one full 3bit value 101 which serves as the new "max possible corrections performed"
- so an enum with these different statuses, they mostly overlap, but there would need to be separation based on how many ECC status bits are available
- expose the status for some internal Dhara metadata marking -- it can be below Dhara: nand read -> check status -> if status != 00, write to page OOB area (the part protected by internal ECC) in flash-friendly-can-be-updated way
- then dhara write/find next user page/head whatever can be made to check those OOB bits and have some e.g. 50/50% - 75/25% chance to "relieve" the page for this write and skip it?
### OBSOLETE END
- !! we already have that https://github.com/espressif/idf-extra-components/blob/master/spi_nand_flash/priv_include/nand.h#L28
- the status is checked only on not corrected https://github.com/espressif/idf-extra-components/blob/master/spi_nand_flash/src/dhara_glue.c#L231
- https://github.com/espressif/idf-extra-components/blob/master/spi_nand_flash/src/nand.c#L255 rewrite of data to the same sector is already done on soft ECC error
- nand_diag_api.c: so it's kinda already there, handle->chip.ecc_data is being set: is_ecc_error() parses status register, read_page_and_wait sets status
- so it's read_page_and_wait() -> is_ecc_error()
- https://github.com/espressif/idf-extra-components/blob/master/spi_nand_flash/src/nand_impl.c#L295 why is the page read 2 times in different ways? is the first read just a mock without getting the data but to trigger ECC correction and updating of status register? yessir
- https://github.com/espressif/idf-extra-components/blob/master/spi_nand_flash/src/nand_impl.c#L235 `used_marker`? another 2 bytes after bad block marker?
- TODO MT29F4G01ABAFDWB, MT29F4G01ABAFD12 looks like it has 4 bytes reserved for bad block marking, so it's not _really_ valid we're writing there?
- also that first spare area is not internal ecc protected -- don't we wanna write to oob that is? if we check first 2 bytes to determine bad block, if it's bad, we don't care about anything else, if not, the next 2 bytes ought to be 0xffff. but still feels bad writing to possibly reserved non ecc-protected space for a marker
- it varies between NAND chips... the layout of spare areas, ecc prot vs non-prot, the size of bad block marker... should we tailor it to each nand specifically? assuming there won't be changes between chips from a single vendor
- PROGRAM LOAD RANDOM DATA 0x84 command loads data into cache, then PROGRAM EXECUTE writes to flash