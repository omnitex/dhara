journal.h: struct dhara_journal, epoch, increments whenever journal head passes the end of chip
and wraps around. So the "perfect wear leveling, only off by 1" is achieved via going through
the memory and using always a new page for writing? 

log2_ppc: it's the log2 of ppc (pages per checkpoint) of the number of contiguous aligned pages in one "checkpoint" -- a unit in the journal user data is grouped into

journal.c: dhara_journal_init: choosing ppc based on log2 of page size and log2 of ppb (pages per  erase block). choose_ppc "calculate checkpoint period"

choose_ppc(log2_page_sie, max) ~ (log2_page_size, log2_ppb)


trace_path() somehow reconstructs something from the journal? but is the journal
physically stored? in what form? how is it reconstructed on start-up?

on a higher lvl, if it goes in a circular motion, what are the pros and cons of that approach?

!! map_internals.txt !!

Mapping based on binary radix tree. Cannot modify existing objects (flash!) so allocating new LEAF and full path to root, including new ROOT and pointing at older but still valid structures. 4 bit addresses have ROOT + 4 levels and you need to always construct new leaf and full path including root.

Each modification requires `1 + |number of address bits|` (leaf + path up to including root) nodes == pack these into a fixed sized allocation unit.

Alt pointers are pointers downwards from root not on the path to the new sector (those can be inferred (?)).


pros: 
- "perfect wear leveling" (max diff by 1 erase count)
- journal is assumed to be robust. any way to confirm/check? smt like partial verification? out of scope most likely?
- some ECC software implementation for potential strengthening the on-chip ECC

TODO: classify common on device NAND flash ECC strength (relevant bit error rates, uncorrectable, standard requirements etc.)

cons?:
- radix tree queue & dequeue journal, alt-pointers, garbage collection, repacking == quite intricate implementation when compared to legacy 
- if each page write "emits" a journal update, that's slow but safe. potential (dynamic maybe? where to get heuristics) settings?
  e.g. combine multiple requests to a batch? if they come in some time window or something?
- currently the metadata (what is that exactly idk yet) does not use any (?) "OOB" areas, meaning it uses the main user areas of the pages,
  not any spare areas that are accessible (and have from what I've seen quite a fixed structure, is that from ONFI?)