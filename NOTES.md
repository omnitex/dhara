journal.h: struct dhara_journal, epoch, increments whenever journal head passes the end of chip
and wraps around. So the "perfect wear leveling, only off by 1" is achieved via going through
the memory and using always a new page for writing? 

so then the trace_path() somehow reconstructs something from the journal? but is the journal
physically stored? in what form? how is it reconstructed on start-up?

on a higher lvl, if it goes in a circular motion, what are the pros and cons of that approach?

pros: 
- "perfect wear leveling"
- simple idea, theoretically simpler than fully arbitrary mapping layer but the journal complicates things
- journal is assumed to be robust, any way to confirm/check? smt like partial verification? out of scope most likely?

cons?:
- if each page write "emits" a journal update, that's slow but safe. potential (dynamic maybe? where to get heuristics) settings?
  e.g. combine multiple requests to a batch? if they come in some time window or something?
- currently the metadata (what is that exactly idk yet) does not use any (?) "OOB" areas, meaning it uses the main user areas of the pages,
  not any spare areas that are accessible (and have from what I've seen quite a fixed structure, is that from ONFI?)