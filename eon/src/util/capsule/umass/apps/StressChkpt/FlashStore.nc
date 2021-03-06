/*
 * Test Compaction
 */
includes common_header;
includes sizes;

configuration FlashStore {
	provides 
	{
		interface StdControl;
		interface SingleStream;
	}

}

implementation {
    components FlashStoreM, ChunkStorageC, FlashM, FalC, ConsoleC,
               LedsC as Leds, StackC, HPLUARTC, Crc8C,
               SingleStreamC, BundleIndexC, CheckpointC, RootDirectoryC;

    StdControl -> FlashStoreM;
    StdControl -> FalC;
    StdControl -> ChunkStorageC;

	SingleStream = SingleStreamC;
	
    /* Wire the chunk storage system */
    //FlashM -> PageEEPROMC.PageEEPROM[unique("FAL")];
    ChunkStorageC.GenericFlash -> FalC.GenericFlash[unique("Flash")]; 
    ChunkStorageC.Leds -> Leds;
    ChunkStorageC.Crc8 -> Crc8C;

    /* Wire the data structures */
    SingleStreamC.ChunkStorage -> ChunkStorageC.ChunkStorage[unique("Fal")];
    SingleStreamC.Stack -> StackC.Stack[unique("Stack")];
    StackC.ChunkStorage -> ChunkStorageC.ChunkStorage[unique("Fal")];

	BundleIndexC.ChunkStorage -> ChunkStorageC.ChunkStorage[unique("Fal")];
	BundleIndexC.Array -> ArrayC.Array[unique("Array")];
	BundleIndexC.SingleCompaction -> SingleStreamC.SingleCompaction;
	
    /* Debugging */
    StackC.Leds -> Leds;
	ArrayC.Leds -> Leds;
	SingleStreamC.Leds -> Leds;
	BundleIndexC.Leds -> Leds;
	
    CheckpointC.Leds -> Leds;
    
    FlashStoreM.Console -> ConsoleC;
	BundleIndexC.Console -> ConsoleC;
	SingleStreamC.Console -> ConsoleC;
    StackC.Console -> ConsoleC;
	ArrayC.Console -> ConsoleC;
    ChunkStorageC.Console -> ConsoleC;
    RootDirectoryC.Console -> ConsoleC;
    CheckpointC.Console -> ConsoleC;
    ConsoleC.HPLUART -> HPLUARTC;
    
    /* Checkpointing */
    CheckpointC.ChunkStorage -> ChunkStorageC.ChunkStorage[unique("Fal")];
    CheckpointC.Stack -> StackC.Stack[unique("Stack")];
    CheckpointC.RootPtrAccess -> StackC.RootPtrAccess[0];
    CheckpointC.RootDirectory -> RootDirectoryC;

    CheckpointC.Serialize -> ChunkStorageC.Serialize;
    CheckpointC.Serialize -> BundleIndexC.Serialize[0];

    /* Compaction */
    ChunkStorageC.Compaction -> BundleIndexC.Compaction[0];

    /* Application */
    //StressTestC.ChunkStorage -> ChunkStorageC.ChunkStorage[unique("Fal")];
    StressTestC.Leds -> Leds;
    //StressTestC.GenericFlash -> FlashM.GenericFlash[unique("Flash")];
    //StressTestC.Index -> BundleIndexC.Index[unique("BundleIndex")];
    //StressTestC.Checkpoint -> CheckpointC.Checkpoint;
    StressTestC.RootDirectory -> RootDirectoryC;
}
