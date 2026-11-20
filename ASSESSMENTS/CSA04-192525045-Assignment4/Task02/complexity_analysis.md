# Task 2: Algorithmic Complexity Analysis for File Allocation

**Course:** CSA04 — Operating Systems | Assessment 4 (CO4)  
**Student:** S. Sachin | **Register Number:** 192525045  
**Topic:** Big-O Time and Space Complexity Comparison of File-Allocation Algorithms  

---

## 1. Contiguous Allocation First-Fit Algorithm

### Algorithm Pseudocode (SIMATS Standard):
```text
Algorithm ContiguousFirstFit
Input: Disk array D of size M, File identifier ID, Requested block count K
Output: Starting block index S or -1 on failure
BEGIN
    run <- 0
    start <- -1
    FOR i <- 0 TO M - 1 DO
        IF D[i] == FREE THEN
            IF run == 0 THEN
                start <- i
            ENDIF
            run <- run + 1
            IF run == K THEN
                FOR j <- start TO start + K - 1 DO
                    D[j] <- ID
                ENDFOR
                RETURN start
            ENDIF
        ELSE
            run <- 0
            start <- -1
        ENDIF
    ENDFOR
    RETURN -1
END
```

### Complexity Formulation:
- **Time Complexity:**
  - **Best Case:** $O(K)$ when a contiguous free run of length $K$ is found at block 0.
  - **Worst Case:** $O(M)$ where $M$ is total disk blocks (must scan the entire volume before determining failure or finding a run at the disk end).
  - **Average Case:** $O(M)$ disk block scans.
- **Space Complexity:** $O(1)$ auxiliary memory (only integer tracking variables `run` and `start`).

---

## 2. Indexed Allocation Algorithm

### Algorithm Pseudocode (SIMATS Standard):
```text
Algorithm IndexedAllocation
Input: Disk array D of size M, File identifier ID, Requested block count K
Output: Index block IDX and allocated block list INDEX[] or -1
BEGIN
    got <- 0
    idx <- -1
    FOR b <- 0 TO M - 1 DO
        IF D[b] == FREE THEN
            IF idx < 0 THEN
                idx <- b
            ELSE
                INDEX[got] <- b
                got <- got + 1
            ENDIF
            IF got == K THEN
                BREAK
            ENDIF
        ENDIF
    ENDFOR
    IF idx < 0 OR got < K THEN
        RETURN -1
    ENDIF
    D[idx] <- TO_LOWERCASE(ID)
    FOR i <- 0 TO K - 1 DO
        D[INDEX[i]] <- ID
    ENDFOR
    RETURN idx
END
```

### Complexity Formulation:
- **Time Complexity:**
  - **Best Case:** $O(K)$ when $K+1$ free blocks are found at the beginning of the disk.
  - **Worst Case:** $O(M)$ when free blocks are scattered to the very end of the disk.
  - **Average Case:** $O(M)$ block table traversal.
- **Space Complexity:** $O(K)$ auxiliary array memory to buffer the allocated block pointers before committing to the index block.

---

## 3. Alternative Concept: Linked Allocation with In-Memory FAT

### Trade-off Evaluation:
- **Block $k$ Traversal Complexity:**
  - *Standard Linked Allocation:* $O(k)$ physical disk seek operations.
  - *In-Memory FAT:* $O(k)$ software memory lookups ($~10\text{ ns}$ per hop) + $O(1)$ physical disk read.
  - *Indexed Allocation:* $O(1)$ or $O(L)$ where $L$ is index tree depth (maximum 3 reads for a 4 TB volume).
- **Memory Footprint for 4 TB Drive ($2^{30}$ blocks of 4 KB):**
  - FAT entry size = 32 bits ($4\text{ bytes}$).
  - Total FAT Table Memory = $2^{30} \times 4\text{ bytes} = 4\text{ GiB RAM}$!
  - *Conclusion:* Holding the entire FAT for a multi-terabyte disk in RAM is excessively costly compared to extent-based indexed inodes that require only a few kilobytes per active file.
