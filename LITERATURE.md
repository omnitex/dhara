## [High-efficient superblock flash translation layer for NAND flash controller](https://ietresearch.onlinelibrary.wiley.com/doi/10.1049/el.2019.3526)
- "satisfy demand of large media" not the main concern for us
- can provide some ideas about strategies that could lead to lowering power consumption

## [Intrablock Wear Leveling to Counter Layer-to-Layer Endurance Variation of 3-D NAND Flash Memory](https://ieeexplore.ieee.org/document/9966490)
- nothing

## [LLSM: A Lifetime-Aware Wear-Leveling for LSM-Tree on NAND Flash Memory](https://sci-hub.st/https://doi.org/10.1109/TCAD.2022.3197542)
- Log-structured merge-tree: indexed access with high insert volume, buffer in smt like b+ tree, flush to disk as immutable sorted component
- LSMT good for key-value, write-optimized, periodic merging of sorted files on disk <- this needs adapting for flash

### M.-C. Yang, Y.-H. Chang, C.-W. Tsao, and P.-C. Huang, “New ERA: New efficient reliability-aware wear leveling for endurance enhancement of flash storage devices,” in Proc. 50th ACM/EDAC/IEEE Design Autom. Conf. (DAC), 2013, pp. 1–6.
- https://sci-hub.st/https://doi.org/10.1145/2463209.2488936
- bit error rate is a better metric for WL than plain program-erase -> error-rate & type aware
- Write Error Table keeps the maximum encountered bit errors corrected by ECC per each block (in RAM)
- set lower and higher thresholds for encountered bit errors, either migrate block or mark as bad upon reaching them
- high response time as it (linearly?) scans the list of blocks to always find the least erroneous one? limited by N (maximum number of blocks scanned in each FREEBLK-ALLOC procedure)
- also EACH write operation does read-back-and-check
### Y.-M. Chang, P.-C. Hsiu, Y.-H. Chang, C.-H. Chen, T.-W. Kuo, and C.-Y. M. Wang, “Improving PCM endurance with a constant-cost wear leveling design,” ACM Trans. Design Autom. Electron. Syst., vol. 22, no. 1, pp. 1–27, 2016.
- https://sci-hub.st/https://doi.org/10.1145/2905364
- PCM memory as a DRAM replacement, not NAND flash, but still applicable?
- "constant time search cost"
- nothingburger
### J. Zhang, Y. Lu, J. Shu, and X. Qin, “FlashKV: Accelerating KV performance with open-channel SSDs,” ACM Trans. Embedded Comput. Syst., vol. 16, no. 5s, pp. 1–19, 2017.
- https://sci-hub.st/https://doi.org/10.1145/3126545
- "open channel SSD" == raw flash device without firmware Flash Translation Layer
- per fig. 3, the proposed Flash KV sits on top of open-channel driver which contains WL so the paper does not really propose any WL solution
### S.-W. Cheng, Y.-H. Chang, T.-Y. Chen, Y.-F. Chang, H.-W. Wei, and W.-K. Shih, “Efficient warranty-aware wear leveling for embedded systems with PCM main memory,” IEEE Trans. Very Large Scale Integr. (VLSI) Syst., vol. 24, no. 7, pp. 2535–2547, Jul. 2016.
- https://sci-hub.st/https://doi.org/10.1109/TVLSI.2015.2511147
- warranty-aware meaning? counter cache + page write counters, not that interesting
- eh, it keeps track of global elapsed time to judge how quickly a big amount of writes is induced, to treat hot pages differently I suppose
- doesn't feel it's too groundbreaking to dive deeper into it
### M.-C. Yang, Y.-H. Chang, T.-W. Kuo, and P.-C. Huang, “Capacity-independent address mapping for flash storage devices with explosively growing capacity,” IEEE Trans. Comput., vol. 65, no. 2, pp. 448–465, Feb. 2016.
- https://sci-hub.st/https://doi.org/10.1109/TC.2015.2428702
- RAM space requirement should depend on accessed data set by the user ("working set") instead of the full capacity of flash
- MLC: no partial page programming & pages of a block must be written sequentially from 1st to last!!!
- target is ultra-large-scaled MLC-based flash storage devices
- run-length tree for mapping information for multiple virtual pages together -- write request usually targets multiple consecutive pages
- eh, too wordy I don't grasp the details of the ideas

## [Adaptive Differential Wearing for Read Performance Optimization on High-Density nand Flash Memory](https://ieeexplore.ieee.org/abstract/document/10190114)
- Adaptive Differential Wearing (ADWR), high density 3D NAND, optimizing read-retries -- not really the case for our NANDs (SLC, 60k-100k P/E cycle lifetime per data sheet, very likely not any advanced 3D process)
- we cannot issue read-retry with different voltages, right? SPI NAND does not allow this kind of raw access? (e.i. you just issue a READ command and let the chip return something + status, right?)