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

TODO: real HW benchmark? and/or over abstracted block device? how to classify performance?
 - prolly need to do add verbose "debug prints" to Dhara/dhara_glue/spi_nand_flash and test on HW to capture the flow easier than going through the code

### cons?:
- radix tree queue & dequeue journal, alt-pointers, garbage collection, repacking == quite intricate implementation when compared to legacy (A)
- if each page write "emits" a journal update, "instruction consumption"?
- no read disturbance mitigation (fact check?) -- repeated read operations without refreshing the data via writing degrades it

(A) comparing to "legacy" WL https://github.com/espressif/esp-idf/tree/master/components/wear_levelling
- legacy on NOR flash, 4KB sector size vs. NAND where each page is 2048/4096 + some OOB
- legacy had multiple counters (incrementing at diferent rates based on erase operations performed) + a simple calculation for logical->physical sector to access in the given address space
- legacy did write to flash in minimal chunks of 16 B ("pos update record") !!! NAND can only write a full page, potentially partial-page programming (need to follow datasheet instructions so on-chip ECC works correctly) meaning an absolutely different approach is necessary
- so Dhara does only full page writes, correct? which would be the easiest approach, not fussing over partial page writes
- only 4 partial page programs allowed on a single page
- NOP == "number of partial programs"