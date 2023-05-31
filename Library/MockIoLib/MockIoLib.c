#include <Library/MockIoLib.h>
#include <Library/DebugLib.h>

typedef struct {
  UINT64               Address;
  UINT64               Size;
  REGISTER_SPACE_MOCK  *RegisterMock;
} MOCK_IO_MEMORY_MAP;

GLOBAL_REMOVE_IF_UNREFERENCED  MOCK_IO_MEMORY_MAP  gMockIoMap[5] = {
  {0, 0, NULL},
  {0, 0, NULL},
  {0, 0, NULL},
  {0, 0, NULL},
  {0, 0, NULL},
};

GLOBAL_REMOVE_IF_UNREFERENCED UINT32 gMockIoMapSize = 0;

GLOBAL_REMOVE_IF_UNREFERENCED  MOCK_IO_MEMORY_MAP  gMockMmioMap[5] = {
  {0, 0, NULL},
  {0, 0, NULL},
  {0, 0, NULL},
  {0, 0, NULL},
  {0, 0, NULL},
};

GLOBAL_REMOVE_IF_UNREFERENCED UINT32 gMockMmioMapSize = 0;

REGISTER_SPACE_MOCK*
MockIoGetRegisterSpace (
  IN UINT64               Address,
  IN MOCK_IO_MEMORY_TYPE  MemoryType,
  OUT UINT64              *Offset
)
{
  UINT32  Index;
  MOCK_IO_MEMORY_MAP  *MemoryMap;
  UINT32  MapSize;

  switch (MemoryType) {
    case MockIoTypeIo:
      MemoryMap = gMockIoMap;
      MapSize = gMockIoMapSize;
      break;
    case MockIoTypeMmio:
    default:
      MemoryMap = gMockMmioMap;
      MapSize = gMockMmioMapSize;
      break;
  }

  for (Index = 0; Index < MapSize; Index++) {
    if (Address >= MemoryMap[Index].Address && Address < (MemoryMap[Index].Address + MemoryMap[Index].Size)) {
      *Offset = Address - MemoryMap[Index].Address;
      return MemoryMap[Index].RegisterMock;
    }
  }

  return NULL;
}

EFI_STATUS
MockIoRegisterMmioAtAddress (
  IN REGISTER_SPACE_MOCK *RegisterSpaceMock,
  IN MOCK_IO_MEMORY_TYPE  Type,
  IN UINT64               Address,
  IN UINT64               Size
  )
{
  MOCK_IO_MEMORY_MAP  *Map;
  UINT32              *MapSize;

  switch (Type) {
    case MockIoTypeIo:
      Map = gMockIoMap;
      MapSize = &gMockIoMapSize;
      break;
    case MockIoTypeMmio:
    default:
      Map = gMockMmioMap;
      MapSize = &gMockMmioMapSize;
      break;
  }

  Map[*MapSize].Address = Address;
  Map[*MapSize].Size = Size;
  Map[*MapSize].RegisterMock = RegisterSpaceMock;

  *MapSize = (*MapSize) + 1;

  return EFI_SUCCESS;
}

/**
  Reads an 8-bit I/O port.

  Reads the 8-bit I/O port specified by Port. The 8-bit read value is returned.
  This function must guarantee that all I/O read and write operations are
  serialized.

  If 8-bit I/O port operations are not supported, then ASSERT().

  @param  Port  The I/O port to read.

  @return The value read.

**/
UINT8
EFIAPI
IoRead8 (
  IN      UINTN  Port
  )
{
  REGISTER_SPACE_MOCK  *Mock;
  UINT64               Value;
  UINT64               Offset;

  Mock = MockIoGetRegisterSpace (Port, MockIoTypeIo, &Offset);
  if (Mock == NULL) {
    DEBUG ((DEBUG_INFO, "Mock is null\n"));
    return 0xFF;
  }

  Mock->Read (Mock, Offset, 1, &Value);
  return (UINT8) Value;
}


/**
  Writes an 8-bit I/O port.

  Writes the 8-bit I/O port specified by Port with the value specified by Value
  and returns Value. This function must guarantee that all I/O read and write
  operations are serialized.

  If 8-bit I/O port operations are not supported, then ASSERT().

  @param  Port  The I/O port to write.
  @param  Value The value to write to the I/O port.

  @return The value written the I/O port.

**/
UINT8
EFIAPI
IoWrite8 (
  IN      UINTN  Port,
  IN      UINT8  Value
  )
{
  REGISTER_SPACE_MOCK  *Mock;
  UINT64                Val;
  UINT64               Offset;

  Mock = MockIoGetRegisterSpace (Port, MockIoTypeIo, &Offset);
  if (Mock == NULL) {
    return 0xFF;
  }

  Val = Value;
  Mock->Write (Mock, Offset, 1, Val);
  return Value;
}

/**
  Reads an 8-bit I/O port fifo into a block of memory.

  Reads the 8-bit I/O fifo port specified by Port.
  The port is read Count times, and the read data is
  stored in the provided Buffer.

  This function must guarantee that all I/O read and write operations are
  serialized.

  If 8-bit I/O port operations are not supported, then ASSERT().

  @param  Port    The I/O port to read.
  @param  Count   The number of times to read I/O port.
  @param  Buffer  The buffer to store the read data into.

**/
VOID
EFIAPI
IoReadFifo8 (
  IN      UINTN  Port,
  IN      UINTN  Count,
  OUT     VOID   *Buffer
  )
{

}

/**
  Writes a block of memory into an 8-bit I/O port fifo.

  Writes the 8-bit I/O fifo port specified by Port.
  The port is written Count times, and the write data is
  retrieved from the provided Buffer.

  This function must guarantee that all I/O write and write operations are
  serialized.

  If 8-bit I/O port operations are not supported, then ASSERT().

  @param  Port    The I/O port to write.
  @param  Count   The number of times to write I/O port.
  @param  Buffer  The buffer to retrieve the write data from.

**/
VOID
EFIAPI
IoWriteFifo8 (
  IN      UINTN  Port,
  IN      UINTN  Count,
  IN      VOID   *Buffer
  )
{

}

/**
  Reads an 8-bit I/O port, performs a bitwise OR, and writes the
  result back to the 8-bit I/O port.

  Reads the 8-bit I/O port specified by Port, performs a bitwise OR
  between the read result and the value specified by OrData, and writes the
  result to the 8-bit I/O port specified by Port. The value written to the I/O
  port is returned. This function must guarantee that all I/O read and write
  operations are serialized.

  If 8-bit I/O port operations are not supported, then ASSERT().

  @param  Port    The I/O port to write.
  @param  OrData  The value to OR with the read value from the I/O port.

  @return The value written back to the I/O port.

**/
UINT8
EFIAPI
IoOr8 (
  IN      UINTN  Port,
  IN      UINT8  OrData
  )
{
  UINT8  Val;

  Val = IoRead8 (Port);
  Val |= OrData;
  return IoWrite8 (Port, Val);
}

/**
  Reads an 8-bit I/O port, performs a bitwise AND, and writes the result back
  to the 8-bit I/O port.

  Reads the 8-bit I/O port specified by Port, performs a bitwise AND between
  the read result and the value specified by AndData, and writes the result to
  the 8-bit I/O port specified by Port. The value written to the I/O port is
  returned. This function must guarantee that all I/O read and write operations
  are serialized.

  If 8-bit I/O port operations are not supported, then ASSERT().

  @param  Port    The I/O port to write.
  @param  AndData The value to AND with the read value from the I/O port.

  @return The value written back to the I/O port.

**/
UINT8
EFIAPI
IoAnd8 (
  IN      UINTN  Port,
  IN      UINT8  AndData
  )
{
  UINT8 Val;

  Val = IoRead8 (Port);
  Val &= AndData;
  return IoWrite8 (Port, Val);
}

/**
  Reads an 8-bit I/O port, performs a bitwise AND followed by a bitwise
  OR, and writes the result back to the 8-bit I/O port.

  Reads the 8-bit I/O port specified by Port, performs a bitwise AND between
  the read result and the value specified by AndData, performs a bitwise OR
  between the result of the AND operation and the value specified by OrData,
  and writes the result to the 8-bit I/O port specified by Port. The value
  written to the I/O port is returned. This function must guarantee that all
  I/O read and write operations are serialized.

  If 8-bit I/O port operations are not supported, then ASSERT().

  @param  Port    The I/O port to write.
  @param  AndData The value to AND with the read value from the I/O port.
  @param  OrData  The value to OR with the result of the AND operation.

  @return The value written back to the I/O port.

**/
UINT8
EFIAPI
IoAndThenOr8 (
  IN      UINTN  Port,
  IN      UINT8  AndData,
  IN      UINT8  OrData
  )
{
  UINT8 Val;

  Val = IoRead8 (Port);
  Val &= AndData;
  Val |= OrData;
  return IoWrite8 (Port, Val);
}

/**
  Reads a bit field of an I/O register.

  Reads the bit field in an 8-bit I/O register. The bit field is specified by
  the StartBit and the EndBit. The value of the bit field is returned.

  If 8-bit I/O port operations are not supported, then ASSERT().
  If StartBit is greater than 7, then ASSERT().
  If EndBit is greater than 7, then ASSERT().
  If EndBit is less than StartBit, then ASSERT().

  @param  Port      The I/O port to read.
  @param  StartBit  The ordinal of the least significant bit in the bit field.
                    Range 0..7.
  @param  EndBit    The ordinal of the most significant bit in the bit field.
                    Range 0..7.

  @return The value read.

**/
UINT8
EFIAPI
IoBitFieldRead8 (
  IN      UINTN  Port,
  IN      UINTN  StartBit,
  IN      UINTN  EndBit
  )
{
  return 0xFF;
}

/**
  Writes a bit field to an I/O register.

  Writes Value to the bit field of the I/O register. The bit field is specified
  by the StartBit and the EndBit. All other bits in the destination I/O
  register are preserved. The value written to the I/O port is returned.

  If 8-bit I/O port operations are not supported, then ASSERT().
  If StartBit is greater than 7, then ASSERT().
  If EndBit is greater than 7, then ASSERT().
  If EndBit is less than StartBit, then ASSERT().
  If Value is larger than the bitmask value range specified by StartBit and EndBit, then ASSERT().

  @param  Port      The I/O port to write.
  @param  StartBit  The ordinal of the least significant bit in the bit field.
                    Range 0..7.
  @param  EndBit    The ordinal of the most significant bit in the bit field.
                    Range 0..7.
  @param  Value     New value of the bit field.

  @return The value written back to the I/O port.

**/
UINT8
EFIAPI
IoBitFieldWrite8 (
  IN      UINTN  Port,
  IN      UINTN  StartBit,
  IN      UINTN  EndBit,
  IN      UINT8  Value
  )
{
  return 0xFF;
}

/**
  Reads a bit field in an 8-bit port, performs a bitwise OR, and writes the
  result back to the bit field in the 8-bit port.

  Reads the 8-bit I/O port specified by Port, performs a bitwise OR
  between the read result and the value specified by OrData, and writes the
  result to the 8-bit I/O port specified by Port. The value written to the I/O
  port is returned. This function must guarantee that all I/O read and write
  operations are serialized. Extra left bits in OrData are stripped.

  If 8-bit I/O port operations are not supported, then ASSERT().
  If StartBit is greater than 7, then ASSERT().
  If EndBit is greater than 7, then ASSERT().
  If EndBit is less than StartBit, then ASSERT().
  If OrData is larger than the bitmask value range specified by StartBit and EndBit, then ASSERT().

  @param  Port      The I/O port to write.
  @param  StartBit  The ordinal of the least significant bit in the bit field.
                    Range 0..7.
  @param  EndBit    The ordinal of the most significant bit in the bit field.
                    Range 0..7.
  @param  OrData    The value to OR with the read value from the I/O port.

  @return The value written back to the I/O port.

**/
UINT8
EFIAPI
IoBitFieldOr8 (
  IN      UINTN  Port,
  IN      UINTN  StartBit,
  IN      UINTN  EndBit,
  IN      UINT8  OrData
  )
{
  return 0xFF;
}

/**
  Reads a bit field in an 8-bit port, performs a bitwise AND, and writes the
  result back to the bit field in the 8-bit port.

  Reads the 8-bit I/O port specified by Port, performs a bitwise AND between
  the read result and the value specified by AndData, and writes the result to
  the 8-bit I/O port specified by Port. The value written to the I/O port is
  returned. This function must guarantee that all I/O read and write operations
  are serialized. Extra left bits in AndData are stripped.

  If 8-bit I/O port operations are not supported, then ASSERT().
  If StartBit is greater than 7, then ASSERT().
  If EndBit is greater than 7, then ASSERT().
  If EndBit is less than StartBit, then ASSERT().
  If AndData is larger than the bitmask value range specified by StartBit and EndBit, then ASSERT().

  @param  Port      The I/O port to write.
  @param  StartBit  The ordinal of the least significant bit in the bit field.
                    Range 0..7.
  @param  EndBit    The ordinal of the most significant bit in the bit field.
                    Range 0..7.
  @param  AndData   The value to AND with the read value from the I/O port.

  @return The value written back to the I/O port.

**/
UINT8
EFIAPI
IoBitFieldAnd8 (
  IN      UINTN  Port,
  IN      UINTN  StartBit,
  IN      UINTN  EndBit,
  IN      UINT8  AndData
  )
{
  return 0xFF;
}

/**
  Reads a bit field in an 8-bit port, performs a bitwise AND followed by a
  bitwise OR, and writes the result back to the bit field in the
  8-bit port.

  Reads the 8-bit I/O port specified by Port, performs a bitwise AND followed
  by a bitwise OR between the read result and the value specified by
  AndData, and writes the result to the 8-bit I/O port specified by Port. The
  value written to the I/O port is returned. This function must guarantee that
  all I/O read and write operations are serialized. Extra left bits in both
  AndData and OrData are stripped.

  If 8-bit I/O port operations are not supported, then ASSERT().
  If StartBit is greater than 7, then ASSERT().
  If EndBit is greater than 7, then ASSERT().
  If EndBit is less than StartBit, then ASSERT().
  If AndData is larger than the bitmask value range specified by StartBit and EndBit, then ASSERT().
  If OrData is larger than the bitmask value range specified by StartBit and EndBit, then ASSERT().

  @param  Port      The I/O port to write.
  @param  StartBit  The ordinal of the least significant bit in the bit field.
                    Range 0..7.
  @param  EndBit    The ordinal of the most significant bit in the bit field.
                    Range 0..7.
  @param  AndData   The value to AND with the read value from the I/O port.
  @param  OrData    The value to OR with the result of the AND operation.

  @return The value written back to the I/O port.

**/
UINT8
EFIAPI
IoBitFieldAndThenOr8 (
  IN      UINTN  Port,
  IN      UINTN  StartBit,
  IN      UINTN  EndBit,
  IN      UINT8  AndData,
  IN      UINT8  OrData
  )
{
  return 0xFF;
}

/**
  Reads a 16-bit I/O port.

  Reads the 16-bit I/O port specified by Port. The 16-bit read value is returned.
  This function must guarantee that all I/O read and write operations are
  serialized.

  If 16-bit I/O port operations are not supported, then ASSERT().
  If Port is not aligned on a 16-bit boundary, then ASSERT().

  @param  Port  The I/O port to read.

  @return The value read.

**/
UINT16
EFIAPI
IoRead16 (
  IN      UINTN  Port
  )
{
  REGISTER_SPACE_MOCK  *Mock;
  UINT64               Value;
  UINT64               Offset;

  Mock = MockIoGetRegisterSpace (Port, MockIoTypeIo, &Offset);
  if (Mock == NULL) {
    return 0xFFFF;
  }

  Mock->Read (Mock, Offset, 2, &Value);
  return (UINT16) Value;
}

/**
  Writes a 16-bit I/O port.

  Writes the 16-bit I/O port specified by Port with the value specified by Value
  and returns Value. This function must guarantee that all I/O read and write
  operations are serialized.

  If 16-bit I/O port operations are not supported, then ASSERT().
  If Port is not aligned on a 16-bit boundary, then ASSERT().

  @param  Port  The I/O port to write.
  @param  Value The value to write to the I/O port.

  @return The value written the I/O port.

**/
UINT16
EFIAPI
IoWrite16 (
  IN      UINTN   Port,
  IN      UINT16  Value
  )
{
  REGISTER_SPACE_MOCK  *Mock;
  UINT64                Val;
  UINT64                Offset;

  Mock = MockIoGetRegisterSpace (Port, MockIoTypeIo, &Offset);
  if (Mock == NULL) {
    return 0xFFFF;
  }

  Val = Value;
  Mock->Write (Mock, Offset, 2, Val);
  return Value;
}

/**
  Reads a 16-bit I/O port fifo into a block of memory.

  Reads the 16-bit I/O fifo port specified by Port.
  The port is read Count times, and the read data is
  stored in the provided Buffer.

  This function must guarantee that all I/O read and write operations are
  serialized.

  If 16-bit I/O port operations are not supported, then ASSERT().

  @param  Port    The I/O port to read.
  @param  Count   The number of times to read I/O port.
  @param  Buffer  The buffer to store the read data into.

**/
VOID
EFIAPI
IoReadFifo16 (
  IN      UINTN  Port,
  IN      UINTN  Count,
  OUT     VOID   *Buffer
  )
{

}

/**
  Writes a block of memory into a 16-bit I/O port fifo.

  Writes the 16-bit I/O fifo port specified by Port.
  The port is written Count times, and the write data is
  retrieved from the provided Buffer.

  This function must guarantee that all I/O write and write operations are
  serialized.

  If 16-bit I/O port operations are not supported, then ASSERT().

  @param  Port    The I/O port to write.
  @param  Count   The number of times to write I/O port.
  @param  Buffer  The buffer to retrieve the write data from.

**/
VOID
EFIAPI
IoWriteFifo16 (
  IN      UINTN  Port,
  IN      UINTN  Count,
  IN      VOID   *Buffer
  )
{}

/**
  Reads a 16-bit I/O port, performs a bitwise OR, and writes the
  result back to the 16-bit I/O port.

  Reads the 16-bit I/O port specified by Port, performs a bitwise OR
  between the read result and the value specified by OrData, and writes the
  result to the 16-bit I/O port specified by Port. The value written to the I/O
  port is returned. This function must guarantee that all I/O read and write
  operations are serialized.

  If 16-bit I/O port operations are not supported, then ASSERT().
  If Port is not aligned on a 16-bit boundary, then ASSERT().

  @param  Port    The I/O port to write.
  @param  OrData  The value to OR with the read value from the I/O port.

  @return The value written back to the I/O port.

**/
UINT16
EFIAPI
IoOr16 (
  IN      UINTN   Port,
  IN      UINT16  OrData
  )
{
  UINT16  Val;

  Val = IoRead16 (Port);
  Val |= OrData;
  return IoWrite16 (Port, Val);
}

/**
  Reads a 16-bit I/O port, performs a bitwise AND, and writes the result back
  to the 16-bit I/O port.

  Reads the 16-bit I/O port specified by Port, performs a bitwise AND between
  the read result and the value specified by AndData, and writes the result to
  the 16-bit I/O port specified by Port. The value written to the I/O port is
  returned. This function must guarantee that all I/O read and write operations
  are serialized.

  If 16-bit I/O port operations are not supported, then ASSERT().
  If Port is not aligned on a 16-bit boundary, then ASSERT().

  @param  Port    The I/O port to write.
  @param  AndData The value to AND with the read value from the I/O port.

  @return The value written back to the I/O port.

**/
UINT16
EFIAPI
IoAnd16 (
  IN      UINTN   Port,
  IN      UINT16  AndData
  )
{
  UINT16  Val;

  Val = IoRead16 (Port);
  Val |= AndData;
  return IoWrite16 (Port, Val);
}

/**
  Reads a 16-bit I/O port, performs a bitwise AND followed by a bitwise
  OR, and writes the result back to the 16-bit I/O port.

  Reads the 16-bit I/O port specified by Port, performs a bitwise AND between
  the read result and the value specified by AndData, performs a bitwise OR
  between the result of the AND operation and the value specified by OrData,
  and writes the result to the 16-bit I/O port specified by Port. The value
  written to the I/O port is returned. This function must guarantee that all
  I/O read and write operations are serialized.

  If 16-bit I/O port operations are not supported, then ASSERT().
  If Port is not aligned on a 16-bit boundary, then ASSERT().

  @param  Port    The I/O port to write.
  @param  AndData The value to AND with the read value from the I/O port.
  @param  OrData  The value to OR with the result of the AND operation.

  @return The value written back to the I/O port.

**/
UINT16
EFIAPI
IoAndThenOr16 (
  IN      UINTN   Port,
  IN      UINT16  AndData,
  IN      UINT16  OrData
  )
{
  UINT16  Val;

  Val = IoRead16 (Port);
  Val &= AndData;
  Val |= OrData;
  return IoWrite16 (Port, Val);
}

/**
  Reads a bit field of an I/O register.

  Reads the bit field in a 16-bit I/O register. The bit field is specified by
  the StartBit and the EndBit. The value of the bit field is returned.

  If 16-bit I/O port operations are not supported, then ASSERT().
  If Port is not aligned on a 16-bit boundary, then ASSERT().
  If StartBit is greater than 15, then ASSERT().
  If EndBit is greater than 15, then ASSERT().
  If EndBit is less than StartBit, then ASSERT().

  @param  Port      The I/O port to read.
  @param  StartBit  The ordinal of the least significant bit in the bit field.
                    Range 0..15.
  @param  EndBit    The ordinal of the most significant bit in the bit field.
                    Range 0..15.

  @return The value read.

**/
UINT16
EFIAPI
IoBitFieldRead16 (
  IN      UINTN  Port,
  IN      UINTN  StartBit,
  IN      UINTN  EndBit
  )
{
  return 0xFFFF;
}

/**
  Writes a bit field to an I/O register.

  Writes Value to the bit field of the I/O register. The bit field is specified
  by the StartBit and the EndBit. All other bits in the destination I/O
  register are preserved. The value written to the I/O port is returned. Extra
  left bits in Value are stripped.

  If 16-bit I/O port operations are not supported, then ASSERT().
  If Port is not aligned on a 16-bit boundary, then ASSERT().
  If StartBit is greater than 15, then ASSERT().
  If EndBit is greater than 15, then ASSERT().
  If EndBit is less than StartBit, then ASSERT().
  If Value is larger than the bitmask value range specified by StartBit and EndBit, then ASSERT().

  @param  Port      The I/O port to write.
  @param  StartBit  The ordinal of the least significant bit in the bit field.
                    Range 0..15.
  @param  EndBit    The ordinal of the most significant bit in the bit field.
                    Range 0..15.
  @param  Value     New value of the bit field.

  @return The value written back to the I/O port.

**/
UINT16
EFIAPI
IoBitFieldWrite16 (
  IN      UINTN   Port,
  IN      UINTN   StartBit,
  IN      UINTN   EndBit,
  IN      UINT16  Value
  )
{
  return 0xFFFF;
}

/**
  Reads a bit field in a 16-bit port, performs a bitwise OR, and writes the
  result back to the bit field in the 16-bit port.

  Reads the 16-bit I/O port specified by Port, performs a bitwise OR
  between the read result and the value specified by OrData, and writes the
  result to the 16-bit I/O port specified by Port. The value written to the I/O
  port is returned. This function must guarantee that all I/O read and write
  operations are serialized. Extra left bits in OrData are stripped.

  If 16-bit I/O port operations are not supported, then ASSERT().
  If Port is not aligned on a 16-bit boundary, then ASSERT().
  If StartBit is greater than 15, then ASSERT().
  If EndBit is greater than 15, then ASSERT().
  If EndBit is less than StartBit, then ASSERT().
  If OrData is larger than the bitmask value range specified by StartBit and EndBit, then ASSERT().

  @param  Port      The I/O port to write.
  @param  StartBit  The ordinal of the least significant bit in the bit field.
                    Range 0..15.
  @param  EndBit    The ordinal of the most significant bit in the bit field.
                    Range 0..15.
  @param  OrData    The value to OR with the read value from the I/O port.

  @return The value written back to the I/O port.

**/
UINT16
EFIAPI
IoBitFieldOr16 (
  IN      UINTN   Port,
  IN      UINTN   StartBit,
  IN      UINTN   EndBit,
  IN      UINT16  OrData
  )
{
  return 0xFFFF;
}

/**
  Reads a bit field in a 16-bit port, performs a bitwise AND, and writes the
  result back to the bit field in the 16-bit port.

  Reads the 16-bit I/O port specified by Port, performs a bitwise AND between
  the read result and the value specified by AndData, and writes the result to
  the 16-bit I/O port specified by Port. The value written to the I/O port is
  returned. This function must guarantee that all I/O read and write operations
  are serialized. Extra left bits in AndData are stripped.

  If 16-bit I/O port operations are not supported, then ASSERT().
  If Port is not aligned on a 16-bit boundary, then ASSERT().
  If StartBit is greater than 15, then ASSERT().
  If EndBit is greater than 15, then ASSERT().
  If EndBit is less than StartBit, then ASSERT().
  If AndData is larger than the bitmask value range specified by StartBit and EndBit, then ASSERT().

  @param  Port      The I/O port to write.
  @param  StartBit  The ordinal of the least significant bit in the bit field.
                    Range 0..15.
  @param  EndBit    The ordinal of the most significant bit in the bit field.
                    Range 0..15.
  @param  AndData   The value to AND with the read value from the I/O port.

  @return The value written back to the I/O port.

**/
UINT16
EFIAPI
IoBitFieldAnd16 (
  IN      UINTN   Port,
  IN      UINTN   StartBit,
  IN      UINTN   EndBit,
  IN      UINT16  AndData
  )
{
  return 0xFFFF;
}

/**
  Reads a bit field in a 16-bit port, performs a bitwise AND followed by a
  bitwise OR, and writes the result back to the bit field in the
  16-bit port.

  Reads the 16-bit I/O port specified by Port, performs a bitwise AND followed
  by a bitwise OR between the read result and the value specified by
  AndData, and writes the result to the 16-bit I/O port specified by Port. The
  value written to the I/O port is returned. This function must guarantee that
  all I/O read and write operations are serialized. Extra left bits in both
  AndData and OrData are stripped.

  If 16-bit I/O port operations are not supported, then ASSERT().
  If Port is not aligned on a 16-bit boundary, then ASSERT().
  If StartBit is greater than 15, then ASSERT().
  If EndBit is greater than 15, then ASSERT().
  If EndBit is less than StartBit, then ASSERT().
  If AndData is larger than the bitmask value range specified by StartBit and EndBit, then ASSERT().
  If OrData is larger than the bitmask value range specified by StartBit and EndBit, then ASSERT().

  @param  Port      The I/O port to write.
  @param  StartBit  The ordinal of the least significant bit in the bit field.
                    Range 0..15.
  @param  EndBit    The ordinal of the most significant bit in the bit field.
                    Range 0..15.
  @param  AndData   The value to AND with the read value from the I/O port.
  @param  OrData    The value to OR with the result of the AND operation.

  @return The value written back to the I/O port.

**/
UINT16
EFIAPI
IoBitFieldAndThenOr16 (
  IN      UINTN   Port,
  IN      UINTN   StartBit,
  IN      UINTN   EndBit,
  IN      UINT16  AndData,
  IN      UINT16  OrData
  )
{
  return 0xFFFF;
}

/**
  Reads a 32-bit I/O port.

  Reads the 32-bit I/O port specified by Port. The 32-bit read value is returned.
  This function must guarantee that all I/O read and write operations are
  serialized.

  If 32-bit I/O port operations are not supported, then ASSERT().
  If Port is not aligned on a 32-bit boundary, then ASSERT().

  @param  Port  The I/O port to read.

  @return The value read.

**/
UINT32
EFIAPI
IoRead32 (
  IN      UINTN  Port
  )
{
  REGISTER_SPACE_MOCK  *Mock;
  UINT64               Value;
  UINT64               Offset;

  Mock = MockIoGetRegisterSpace (Port, MockIoTypeIo, &Offset);
  if (Mock == NULL) {
    return 0xFFFFFFFF;
  }

  Mock->Read (Mock, Offset, 4, &Value);
  return (UINT32) Value;
}

/**
  Writes a 32-bit I/O port.

  Writes the 32-bit I/O port specified by Port with the value specified by Value
  and returns Value. This function must guarantee that all I/O read and write
  operations are serialized.

  If 32-bit I/O port operations are not supported, then ASSERT().
  If Port is not aligned on a 32-bit boundary, then ASSERT().

  @param  Port  The I/O port to write.
  @param  Value The value to write to the I/O port.

  @return The value written the I/O port.

**/
UINT32
EFIAPI
IoWrite32 (
  IN      UINTN   Port,
  IN      UINT32  Value
  )
{
  REGISTER_SPACE_MOCK  *Mock;
  UINT64                Val;
  UINT64               Offset;

  Mock = MockIoGetRegisterSpace (Port, MockIoTypeIo, &Offset);
  if (Mock == NULL) {
    return 0xFFFFFFFF;
  }

  Val = Value;
  Mock->Write (Mock, Offset, 4, Val);
  return Value;
}

/**
  Reads a 32-bit I/O port fifo into a block of memory.

  Reads the 32-bit I/O fifo port specified by Port.
  The port is read Count times, and the read data is
  stored in the provided Buffer.

  This function must guarantee that all I/O read and write operations are
  serialized.

  If 32-bit I/O port operations are not supported, then ASSERT().

  @param  Port    The I/O port to read.
  @param  Count   The number of times to read I/O port.
  @param  Buffer  The buffer to store the read data into.

**/
VOID
EFIAPI
IoReadFifo32 (
  IN      UINTN  Port,
  IN      UINTN  Count,
  OUT     VOID   *Buffer
  )
{

}

/**
  Writes a block of memory into a 32-bit I/O port fifo.

  Writes the 32-bit I/O fifo port specified by Port.
  The port is written Count times, and the write data is
  retrieved from the provided Buffer.

  This function must guarantee that all I/O write and write operations are
  serialized.

  If 32-bit I/O port operations are not supported, then ASSERT().

  @param  Port    The I/O port to write.
  @param  Count   The number of times to write I/O port.
  @param  Buffer  The buffer to retrieve the write data from.

**/
VOID
EFIAPI
IoWriteFifo32 (
  IN      UINTN  Port,
  IN      UINTN  Count,
  IN      VOID   *Buffer
  )
{

}

/**
  Reads a 32-bit I/O port, performs a bitwise OR, and writes the
  result back to the 32-bit I/O port.

  Reads the 32-bit I/O port specified by Port, performs a bitwise OR
  between the read result and the value specified by OrData, and writes the
  result to the 32-bit I/O port specified by Port. The value written to the I/O
  port is returned. This function must guarantee that all I/O read and write
  operations are serialized.

  If 32-bit I/O port operations are not supported, then ASSERT().
  If Port is not aligned on a 32-bit boundary, then ASSERT().

  @param  Port    The I/O port to write.
  @param  OrData  The value to OR with the read value from the I/O port.

  @return The value written back to the I/O port.

**/
UINT32
EFIAPI
IoOr32 (
  IN      UINTN   Port,
  IN      UINT32  OrData
  )
{
  UINT32  Val;

  Val = IoRead32 (Port);
  Val |= OrData;
  return IoWrite32 (Port, Val);
}

/**
  Reads a 32-bit I/O port, performs a bitwise AND, and writes the result back
  to the 32-bit I/O port.

  Reads the 32-bit I/O port specified by Port, performs a bitwise AND between
  the read result and the value specified by AndData, and writes the result to
  the 32-bit I/O port specified by Port. The value written to the I/O port is
  returned. This function must guarantee that all I/O read and write operations
  are serialized.

  If 32-bit I/O port operations are not supported, then ASSERT().
  If Port is not aligned on a 32-bit boundary, then ASSERT().

  @param  Port    The I/O port to write.
  @param  AndData The value to AND with the read value from the I/O port.

  @return The value written back to the I/O port.

**/
UINT32
EFIAPI
IoAnd32 (
  IN      UINTN   Port,
  IN      UINT32  AndData
  )
{
  UINT32  Val;

  Val = IoRead32 (Port);
  Val &= AndData;
  return IoWrite32 (Port, Val);
}

/**
  Reads a 32-bit I/O port, performs a bitwise AND followed by a bitwise
  OR, and writes the result back to the 32-bit I/O port.

  Reads the 32-bit I/O port specified by Port, performs a bitwise AND between
  the read result and the value specified by AndData, performs a bitwise OR
  between the result of the AND operation and the value specified by OrData,
  and writes the result to the 32-bit I/O port specified by Port. The value
  written to the I/O port is returned. This function must guarantee that all
  I/O read and write operations are serialized.

  If 32-bit I/O port operations are not supported, then ASSERT().
  If Port is not aligned on a 32-bit boundary, then ASSERT().

  @param  Port    The I/O port to write.
  @param  AndData The value to AND with the read value from the I/O port.
  @param  OrData  The value to OR with the result of the AND operation.

  @return The value written back to the I/O port.

**/
UINT32
EFIAPI
IoAndThenOr32 (
  IN      UINTN   Port,
  IN      UINT32  AndData,
  IN      UINT32  OrData
  )
{
  UINT32  Val;

  Val = IoRead32 (Port);
  Val &= AndData;
  Val |= OrData;
  return IoWrite32 (Port, Val);
}

/**
  Reads a bit field of an I/O register.

  Reads the bit field in a 32-bit I/O register. The bit field is specified by
  the StartBit and the EndBit. The value of the bit field is returned.

  If 32-bit I/O port operations are not supported, then ASSERT().
  If Port is not aligned on a 32-bit boundary, then ASSERT().
  If StartBit is greater than 31, then ASSERT().
  If EndBit is greater than 31, then ASSERT().
  If EndBit is less than StartBit, then ASSERT().

  @param  Port      The I/O port to read.
  @param  StartBit  The ordinal of the least significant bit in the bit field.
                    Range 0..31.
  @param  EndBit    The ordinal of the most significant bit in the bit field.
                    Range 0..31.

  @return The value read.

**/
UINT32
EFIAPI
IoBitFieldRead32 (
  IN      UINTN  Port,
  IN      UINTN  StartBit,
  IN      UINTN  EndBit
  )
{
  return 0xFFFFFFFF;
}

/**
  Writes a bit field to an I/O register.

  Writes Value to the bit field of the I/O register. The bit field is specified
  by the StartBit and the EndBit. All other bits in the destination I/O
  register are preserved. The value written to the I/O port is returned. Extra
  left bits in Value are stripped.

  If 32-bit I/O port operations are not supported, then ASSERT().
  If Port is not aligned on a 32-bit boundary, then ASSERT().
  If StartBit is greater than 31, then ASSERT().
  If EndBit is greater than 31, then ASSERT().
  If EndBit is less than StartBit, then ASSERT().
  If Value is larger than the bitmask value range specified by StartBit and EndBit, then ASSERT().

  @param  Port      The I/O port to write.
  @param  StartBit  The ordinal of the least significant bit in the bit field.
                    Range 0..31.
  @param  EndBit    The ordinal of the most significant bit in the bit field.
                    Range 0..31.
  @param  Value     New value of the bit field.

  @return The value written back to the I/O port.

**/
UINT32
EFIAPI
IoBitFieldWrite32 (
  IN      UINTN   Port,
  IN      UINTN   StartBit,
  IN      UINTN   EndBit,
  IN      UINT32  Value
  )
{
  return 0xFFFFFFFF;
}

/**
  Reads a bit field in a 32-bit port, performs a bitwise OR, and writes the
  result back to the bit field in the 32-bit port.

  Reads the 32-bit I/O port specified by Port, performs a bitwise OR
  between the read result and the value specified by OrData, and writes the
  result to the 32-bit I/O port specified by Port. The value written to the I/O
  port is returned. This function must guarantee that all I/O read and write
  operations are serialized. Extra left bits in OrData are stripped.

  If 32-bit I/O port operations are not supported, then ASSERT().
  If Port is not aligned on a 32-bit boundary, then ASSERT().
  If StartBit is greater than 31, then ASSERT().
  If EndBit is greater than 31, then ASSERT().
  If EndBit is less than StartBit, then ASSERT().
  If OrData is larger than the bitmask value range specified by StartBit and EndBit, then ASSERT().

  @param  Port      The I/O port to write.
  @param  StartBit  The ordinal of the least significant bit in the bit field.
                    Range 0..31.
  @param  EndBit    The ordinal of the most significant bit in the bit field.
                    Range 0..31.
  @param  OrData    The value to OR with the read value from the I/O port.

  @return The value written back to the I/O port.

**/
UINT32
EFIAPI
IoBitFieldOr32 (
  IN      UINTN   Port,
  IN      UINTN   StartBit,
  IN      UINTN   EndBit,
  IN      UINT32  OrData
  )
{
  return 0xFFFFFFFF;
}

/**
  Reads a bit field in a 32-bit port, performs a bitwise AND, and writes the
  result back to the bit field in the 32-bit port.

  Reads the 32-bit I/O port specified by Port, performs a bitwise AND between
  the read result and the value specified by AndData, and writes the result to
  the 32-bit I/O port specified by Port. The value written to the I/O port is
  returned. This function must guarantee that all I/O read and write operations
  are serialized. Extra left bits in AndData are stripped.

  If 32-bit I/O port operations are not supported, then ASSERT().
  If Port is not aligned on a 32-bit boundary, then ASSERT().
  If StartBit is greater than 31, then ASSERT().
  If EndBit is greater than 31, then ASSERT().
  If EndBit is less than StartBit, then ASSERT().
  If AndData is larger than the bitmask value range specified by StartBit and EndBit, then ASSERT().

  @param  Port      The I/O port to write.
  @param  StartBit  The ordinal of the least significant bit in the bit field.
                    Range 0..31.
  @param  EndBit    The ordinal of the most significant bit in the bit field.
                    Range 0..31.
  @param  AndData   The value to AND with the read value from the I/O port.

  @return The value written back to the I/O port.

**/
UINT32
EFIAPI
IoBitFieldAnd32 (
  IN      UINTN   Port,
  IN      UINTN   StartBit,
  IN      UINTN   EndBit,
  IN      UINT32  AndData
  )
{
  return 0xFFFFFFFF;
}

/**
  Reads a bit field in a 32-bit port, performs a bitwise AND followed by a
  bitwise OR, and writes the result back to the bit field in the
  32-bit port.

  Reads the 32-bit I/O port specified by Port, performs a bitwise AND followed
  by a bitwise OR between the read result and the value specified by
  AndData, and writes the result to the 32-bit I/O port specified by Port. The
  value written to the I/O port is returned. This function must guarantee that
  all I/O read and write operations are serialized. Extra left bits in both
  AndData and OrData are stripped.

  If 32-bit I/O port operations are not supported, then ASSERT().
  If Port is not aligned on a 32-bit boundary, then ASSERT().
  If StartBit is greater than 31, then ASSERT().
  If EndBit is greater than 31, then ASSERT().
  If EndBit is less than StartBit, then ASSERT().
  If AndData is larger than the bitmask value range specified by StartBit and EndBit, then ASSERT().
  If OrData is larger than the bitmask value range specified by StartBit and EndBit, then ASSERT().

  @param  Port      The I/O port to write.
  @param  StartBit  The ordinal of the least significant bit in the bit field.
                    Range 0..31.
  @param  EndBit    The ordinal of the most significant bit in the bit field.
                    Range 0..31.
  @param  AndData   The value to AND with the read value from the I/O port.
  @param  OrData    The value to OR with the result of the AND operation.

  @return The value written back to the I/O port.

**/
UINT32
EFIAPI
IoBitFieldAndThenOr32 (
  IN      UINTN   Port,
  IN      UINTN   StartBit,
  IN      UINTN   EndBit,
  IN      UINT32  AndData,
  IN      UINT32  OrData
  )
{
  return 0xFFFFFFFF;
}

/**
  Reads a 64-bit I/O port.

  Reads the 64-bit I/O port specified by Port. The 64-bit read value is returned.
  This function must guarantee that all I/O read and write operations are
  serialized.

  If 64-bit I/O port operations are not supported, then ASSERT().
  If Port is not aligned on a 64-bit boundary, then ASSERT().

  @param  Port  The I/O port to read.

  @return The value read.

**/
UINT64
EFIAPI
IoRead64 (
  IN      UINTN  Port
  )
{
  REGISTER_SPACE_MOCK  *Mock;
  UINT64               Value;
  UINT64               Offset;

  Mock = MockIoGetRegisterSpace (Port, MockIoTypeIo, &Offset);
  if (Mock == NULL) {
    return 0xFFFFFFFFFFFFFFFF;
  }

  Mock->Read (Mock, Offset, 8, &Value);
  return (UINT64) Value;
}

/**
  Writes a 64-bit I/O port.

  Writes the 64-bit I/O port specified by Port with the value specified by Value
  and returns Value. This function must guarantee that all I/O read and write
  operations are serialized.

  If 64-bit I/O port operations are not supported, then ASSERT().
  If Port is not aligned on a 64-bit boundary, then ASSERT().

  @param  Port  The I/O port to write.
  @param  Value The value to write to the I/O port.

  @return The value written the I/O port.

**/
UINT64
EFIAPI
IoWrite64 (
  IN      UINTN   Port,
  IN      UINT64  Value
  )
{
  REGISTER_SPACE_MOCK  *Mock;
  UINT64               Offset;

  Mock = MockIoGetRegisterSpace (Port, MockIoTypeIo, &Offset);
  if (Mock == NULL) {
    return 0xFFFFFFFFFFFFFFFF;
  }

  Mock->Write (Mock, Offset, 8, Value);
  return Value;
}

/**
  Reads a 64-bit I/O port, performs a bitwise OR, and writes the
  result back to the 64-bit I/O port.

  Reads the 64-bit I/O port specified by Port, performs a bitwise OR
  between the read result and the value specified by OrData, and writes the
  result to the 64-bit I/O port specified by Port. The value written to the I/O
  port is returned. This function must guarantee that all I/O read and write
  operations are serialized.

  If 64-bit I/O port operations are not supported, then ASSERT().
  If Port is not aligned on a 64-bit boundary, then ASSERT().

  @param  Port    The I/O port to write.
  @param  OrData  The value to OR with the read value from the I/O port.

  @return The value written back to the I/O port.

**/
UINT64
EFIAPI
IoOr64 (
  IN      UINTN   Port,
  IN      UINT64  OrData
  )
{
  UINT64  Val;

  Val = IoRead64 (Port);
  Val |= OrData;
  return IoWrite64 (Port, Val);
}

/**
  Reads a 64-bit I/O port, performs a bitwise AND, and writes the result back
  to the 64-bit I/O port.

  Reads the 64-bit I/O port specified by Port, performs a bitwise AND between
  the read result and the value specified by AndData, and writes the result to
  the 64-bit I/O port specified by Port. The value written to the I/O port is
  returned. This function must guarantee that all I/O read and write operations
  are serialized.

  If 64-bit I/O port operations are not supported, then ASSERT().
  If Port is not aligned on a 64-bit boundary, then ASSERT().

  @param  Port    The I/O port to write.
  @param  AndData The value to AND with the read value from the I/O port.

  @return The value written back to the I/O port.

**/
UINT64
EFIAPI
IoAnd64 (
  IN      UINTN   Port,
  IN      UINT64  AndData
  )
{
  UINT64  Val;

  Val = IoRead64 (Port);
  Val &= AndData;
  return IoWrite64 (Port, Val);
}

/**
  Reads a 64-bit I/O port, performs a bitwise AND followed by a bitwise
  OR, and writes the result back to the 64-bit I/O port.

  Reads the 64-bit I/O port specified by Port, performs a bitwise AND between
  the read result and the value specified by AndData, performs a bitwise OR
  between the result of the AND operation and the value specified by OrData,
  and writes the result to the 64-bit I/O port specified by Port. The value
  written to the I/O port is returned. This function must guarantee that all
  I/O read and write operations are serialized.

  If 64-bit I/O port operations are not supported, then ASSERT().
  If Port is not aligned on a 64-bit boundary, then ASSERT().

  @param  Port    The I/O port to write.
  @param  AndData The value to AND with the read value from the I/O port.
  @param  OrData  The value to OR with the result of the AND operation.

  @return The value written back to the I/O port.

**/
UINT64
EFIAPI
IoAndThenOr64 (
  IN      UINTN   Port,
  IN      UINT64  AndData,
  IN      UINT64  OrData
  )
{
  UINT64  Val;

  Val = IoRead64 (Port);
  Val &= AndData;
  Val |= OrData;
  return IoWrite64 (Port, Val);
}

/**
  Reads a bit field of an I/O register.

  Reads the bit field in a 64-bit I/O register. The bit field is specified by
  the StartBit and the EndBit. The value of the bit field is returned.

  If 64-bit I/O port operations are not supported, then ASSERT().
  If Port is not aligned on a 64-bit boundary, then ASSERT().
  If StartBit is greater than 63, then ASSERT().
  If EndBit is greater than 63, then ASSERT().
  If EndBit is less than StartBit, then ASSERT().

  @param  Port      The I/O port to read.
  @param  StartBit  The ordinal of the least significant bit in the bit field.
                    Range 0..63.
  @param  EndBit    The ordinal of the most significant bit in the bit field.
                    Range 0..63.

  @return The value read.

**/
UINT64
EFIAPI
IoBitFieldRead64 (
  IN      UINTN  Port,
  IN      UINTN  StartBit,
  IN      UINTN  EndBit
  )
{
  return 0xFFFFFFFFFFFFFFFF;
}

/**
  Writes a bit field to an I/O register.

  Writes Value to the bit field of the I/O register. The bit field is specified
  by the StartBit and the EndBit. All other bits in the destination I/O
  register are preserved. The value written to the I/O port is returned. Extra
  left bits in Value are stripped.

  If 64-bit I/O port operations are not supported, then ASSERT().
  If Port is not aligned on a 64-bit boundary, then ASSERT().
  If StartBit is greater than 63, then ASSERT().
  If EndBit is greater than 63, then ASSERT().
  If EndBit is less than StartBit, then ASSERT().
  If Value is larger than the bitmask value range specified by StartBit and EndBit, then ASSERT().

  @param  Port      The I/O port to write.
  @param  StartBit  The ordinal of the least significant bit in the bit field.
                    Range 0..63.
  @param  EndBit    The ordinal of the most significant bit in the bit field.
                    Range 0..63.
  @param  Value     New value of the bit field.

  @return The value written back to the I/O port.

**/
UINT64
EFIAPI
IoBitFieldWrite64 (
  IN      UINTN   Port,
  IN      UINTN   StartBit,
  IN      UINTN   EndBit,
  IN      UINT64  Value
  )
{
  return 0xFFFFFFFFFFFFFFFF;
}

/**
  Reads a bit field in a 64-bit port, performs a bitwise OR, and writes the
  result back to the bit field in the 64-bit port.

  Reads the 64-bit I/O port specified by Port, performs a bitwise OR
  between the read result and the value specified by OrData, and writes the
  result to the 64-bit I/O port specified by Port. The value written to the I/O
  port is returned. This function must guarantee that all I/O read and write
  operations are serialized. Extra left bits in OrData are stripped.

  If 64-bit I/O port operations are not supported, then ASSERT().
  If Port is not aligned on a 64-bit boundary, then ASSERT().
  If StartBit is greater than 63, then ASSERT().
  If EndBit is greater than 63, then ASSERT().
  If EndBit is less than StartBit, then ASSERT().
  If OrData is larger than the bitmask value range specified by StartBit and EndBit, then ASSERT().

  @param  Port      The I/O port to write.
  @param  StartBit  The ordinal of the least significant bit in the bit field.
                    Range 0..63.
  @param  EndBit    The ordinal of the most significant bit in the bit field.
                    Range 0..63.
  @param  OrData    The value to OR with the read value from the I/O port.

  @return The value written back to the I/O port.

**/
UINT64
EFIAPI
IoBitFieldOr64 (
  IN      UINTN   Port,
  IN      UINTN   StartBit,
  IN      UINTN   EndBit,
  IN      UINT64  OrData
  )
{
  return 0xFFFFFFFFFFFFFFFF;
}

/**
  Reads a bit field in a 64-bit port, performs a bitwise AND, and writes the
  result back to the bit field in the 64-bit port.

  Reads the 64-bit I/O port specified by Port, performs a bitwise AND between
  the read result and the value specified by AndData, and writes the result to
  the 64-bit I/O port specified by Port. The value written to the I/O port is
  returned. This function must guarantee that all I/O read and write operations
  are serialized. Extra left bits in AndData are stripped.

  If 64-bit I/O port operations are not supported, then ASSERT().
  If Port is not aligned on a 64-bit boundary, then ASSERT().
  If StartBit is greater than 63, then ASSERT().
  If EndBit is greater than 63, then ASSERT().
  If EndBit is less than StartBit, then ASSERT().
  If AndData is larger than the bitmask value range specified by StartBit and EndBit, then ASSERT().

  @param  Port      The I/O port to write.
  @param  StartBit  The ordinal of the least significant bit in the bit field.
                    Range 0..63.
  @param  EndBit    The ordinal of the most significant bit in the bit field.
                    Range 0..63.
  @param  AndData   The value to AND with the read value from the I/O port.

  @return The value written back to the I/O port.

**/
UINT64
EFIAPI
IoBitFieldAnd64 (
  IN      UINTN   Port,
  IN      UINTN   StartBit,
  IN      UINTN   EndBit,
  IN      UINT64  AndData
  )
{
  return 0xFFFFFFFFFFFFFFFF;
}

/**
  Reads a bit field in a 64-bit port, performs a bitwise AND followed by a
  bitwise OR, and writes the result back to the bit field in the
  64-bit port.

  Reads the 64-bit I/O port specified by Port, performs a bitwise AND followed
  by a bitwise OR between the read result and the value specified by
  AndData, and writes the result to the 64-bit I/O port specified by Port. The
  value written to the I/O port is returned. This function must guarantee that
  all I/O read and write operations are serialized. Extra left bits in both
  AndData and OrData are stripped.

  If 64-bit I/O port operations are not supported, then ASSERT().
  If Port is not aligned on a 64-bit boundary, then ASSERT().
  If StartBit is greater than 63, then ASSERT().
  If EndBit is greater than 63, then ASSERT().
  If EndBit is less than StartBit, then ASSERT().
  If AndData is larger than the bitmask value range specified by StartBit and EndBit, then ASSERT().
  If OrData is larger than the bitmask value range specified by StartBit and EndBit, then ASSERT().

  @param  Port      The I/O port to write.
  @param  StartBit  The ordinal of the least significant bit in the bit field.
                    Range 0..63.
  @param  EndBit    The ordinal of the most significant bit in the bit field.
                    Range 0..63.
  @param  AndData   The value to AND with the read value from the I/O port.
  @param  OrData    The value to OR with the result of the AND operation.

  @return The value written back to the I/O port.

**/
UINT64
EFIAPI
IoBitFieldAndThenOr64 (
  IN      UINTN   Port,
  IN      UINTN   StartBit,
  IN      UINTN   EndBit,
  IN      UINT64  AndData,
  IN      UINT64  OrData
  )
{
  return 0xFFFFFFFFFFFFFFFF;
}

/**
  Reads an 8-bit MMIO register.

  Reads the 8-bit MMIO register specified by Address. The 8-bit read value is
  returned. This function must guarantee that all MMIO read and write
  operations are serialized.

  If 8-bit MMIO register operations are not supported, then ASSERT().

  @param  Address The MMIO register to read.

  @return The value read.

**/
UINT8
EFIAPI
MmioRead8 (
  IN      UINTN  Address
  )
{
  REGISTER_SPACE_MOCK  *Mock;
  UINT64               Value;
  UINT64               Offset;

  Mock = MockIoGetRegisterSpace (Address, MockIoTypeMmio, &Offset);
  if (Mock == NULL) {
    return 0xFF;
  }

  Mock->Read (Mock, Offset, 1, &Value);
  return (UINT8) Value;
}

/**
  Writes an 8-bit MMIO register.

  Writes the 8-bit MMIO register specified by Address with the value specified
  by Value and returns Value. This function must guarantee that all MMIO read
  and write operations are serialized.

  If 8-bit MMIO register operations are not supported, then ASSERT().

  @param  Address The MMIO register to write.
  @param  Value   The value to write to the MMIO register.

  @return Value.

**/
UINT8
EFIAPI
MmioWrite8 (
  IN      UINTN  Address,
  IN      UINT8  Value
  )
{
  REGISTER_SPACE_MOCK  *Mock;
  UINT64               Val;
  UINT64               Offset;

  Mock = MockIoGetRegisterSpace (Address, MockIoTypeMmio, &Offset);
  if (Mock == NULL) {
    return 0xFF;
  }

  Val = Value;
  Mock->Write (Mock, Offset, 1, Val);
  return (UINT8) Value;
}

/**
  Reads an 8-bit MMIO register, performs a bitwise OR, and writes the
  result back to the 8-bit MMIO register.

  Reads the 8-bit MMIO register specified by Address, performs a bitwise
  OR between the read result and the value specified by OrData, and
  writes the result to the 8-bit MMIO register specified by Address. The value
  written to the MMIO register is returned. This function must guarantee that
  all MMIO read and write operations are serialized.

  If 8-bit MMIO register operations are not supported, then ASSERT().

  @param  Address The MMIO register to write.
  @param  OrData  The value to OR with the read value from the MMIO register.

  @return The value written back to the MMIO register.

**/
UINT8
EFIAPI
MmioOr8 (
  IN      UINTN  Address,
  IN      UINT8  OrData
  )
{
  UINT8  Val;

  Val = MmioRead8 (Address);
  Val |= OrData;
  return MmioWrite8 (Address, Val);
}

/**
  Reads an 8-bit MMIO register, performs a bitwise AND, and writes the result
  back to the 8-bit MMIO register.

  Reads the 8-bit MMIO register specified by Address, performs a bitwise AND
  between the read result and the value specified by AndData, and writes the
  result to the 8-bit MMIO register specified by Address. The value written to
  the MMIO register is returned. This function must guarantee that all MMIO
  read and write operations are serialized.

  If 8-bit MMIO register operations are not supported, then ASSERT().

  @param  Address The MMIO register to write.
  @param  AndData The value to AND with the read value from the MMIO register.

  @return The value written back to the MMIO register.

**/
UINT8
EFIAPI
MmioAnd8 (
  IN      UINTN  Address,
  IN      UINT8  AndData
  )
{
  UINT8  Val;

  Val = MmioRead8 (Address);
  Val &= AndData;
  return MmioWrite8 (Address, Val);
}

/**
  Reads an 8-bit MMIO register, performs a bitwise AND followed by a bitwise
  OR, and writes the result back to the 8-bit MMIO register.

  Reads the 8-bit MMIO register specified by Address, performs a bitwise AND
  between the read result and the value specified by AndData, performs a
  bitwise OR between the result of the AND operation and the value specified by
  OrData, and writes the result to the 8-bit MMIO register specified by
  Address. The value written to the MMIO register is returned. This function
  must guarantee that all MMIO read and write operations are serialized.

  If 8-bit MMIO register operations are not supported, then ASSERT().


  @param  Address The MMIO register to write.
  @param  AndData The value to AND with the read value from the MMIO register.
  @param  OrData  The value to OR with the result of the AND operation.

  @return The value written back to the MMIO register.

**/
UINT8
EFIAPI
MmioAndThenOr8 (
  IN      UINTN  Address,
  IN      UINT8  AndData,
  IN      UINT8  OrData
  )
{
  UINT8  Val;

  Val = MmioRead8 (Address);
  Val &= AndData;
  Val |= OrData;
  return MmioWrite8 (Address, Val);
}

/**
  Reads a bit field of a MMIO register.

  Reads the bit field in an 8-bit MMIO register. The bit field is specified by
  the StartBit and the EndBit. The value of the bit field is returned.

  If 8-bit MMIO register operations are not supported, then ASSERT().
  If StartBit is greater than 7, then ASSERT().
  If EndBit is greater than 7, then ASSERT().
  If EndBit is less than StartBit, then ASSERT().

  @param  Address   MMIO register to read.
  @param  StartBit  The ordinal of the least significant bit in the bit field.
                    Range 0..7.
  @param  EndBit    The ordinal of the most significant bit in the bit field.
                    Range 0..7.

  @return The value read.

**/
UINT8
EFIAPI
MmioBitFieldRead8 (
  IN      UINTN  Address,
  IN      UINTN  StartBit,
  IN      UINTN  EndBit
  )
{
  return 0;
}

/**
  Writes a bit field to a MMIO register.

  Writes Value to the bit field of the MMIO register. The bit field is
  specified by the StartBit and the EndBit. All other bits in the destination
  MMIO register are preserved. The new value of the 8-bit register is returned.

  If 8-bit MMIO register operations are not supported, then ASSERT().
  If StartBit is greater than 7, then ASSERT().
  If EndBit is greater than 7, then ASSERT().
  If EndBit is less than StartBit, then ASSERT().
  If Value is larger than the bitmask value range specified by StartBit and EndBit, then ASSERT().

  @param  Address   MMIO register to write.
  @param  StartBit  The ordinal of the least significant bit in the bit field.
                    Range 0..7.
  @param  EndBit    The ordinal of the most significant bit in the bit field.
                    Range 0..7.
  @param  Value     New value of the bit field.

  @return The value written back to the MMIO register.

**/
UINT8
EFIAPI
MmioBitFieldWrite8 (
  IN      UINTN  Address,
  IN      UINTN  StartBit,
  IN      UINTN  EndBit,
  IN      UINT8  Value
  )
{
  return 0;
}

/**
  Reads a bit field in an 8-bit MMIO register, performs a bitwise OR, and
  writes the result back to the bit field in the 8-bit MMIO register.

  Reads the 8-bit MMIO register specified by Address, performs a bitwise
  OR between the read result and the value specified by OrData, and
  writes the result to the 8-bit MMIO register specified by Address. The value
  written to the MMIO register is returned. This function must guarantee that
  all MMIO read and write operations are serialized. Extra left bits in OrData
  are stripped.

  If 8-bit MMIO register operations are not supported, then ASSERT().
  If StartBit is greater than 7, then ASSERT().
  If EndBit is greater than 7, then ASSERT().
  If EndBit is less than StartBit, then ASSERT().
  If OrData is larger than the bitmask value range specified by StartBit and EndBit, then ASSERT().

  @param  Address   MMIO register to write.
  @param  StartBit  The ordinal of the least significant bit in the bit field.
                    Range 0..7.
  @param  EndBit    The ordinal of the most significant bit in the bit field.
                    Range 0..7.
  @param  OrData    The value to OR with read value from the MMIO register.

  @return The value written back to the MMIO register.

**/
UINT8
EFIAPI
MmioBitFieldOr8 (
  IN      UINTN  Address,
  IN      UINTN  StartBit,
  IN      UINTN  EndBit,
  IN      UINT8  OrData
  )
{
  return 0;
}

/**
  Reads a bit field in an 8-bit MMIO register, performs a bitwise AND, and
  writes the result back to the bit field in the 8-bit MMIO register.

  Reads the 8-bit MMIO register specified by Address, performs a bitwise AND
  between the read result and the value specified by AndData, and writes the
  result to the 8-bit MMIO register specified by Address. The value written to
  the MMIO register is returned. This function must guarantee that all MMIO
  read and write operations are serialized. Extra left bits in AndData are
  stripped.

  If 8-bit MMIO register operations are not supported, then ASSERT().
  If StartBit is greater than 7, then ASSERT().
  If EndBit is greater than 7, then ASSERT().
  If EndBit is less than StartBit, then ASSERT().
  If AndData is larger than the bitmask value range specified by StartBit and EndBit, then ASSERT().

  @param  Address   MMIO register to write.
  @param  StartBit  The ordinal of the least significant bit in the bit field.
                    Range 0..7.
  @param  EndBit    The ordinal of the most significant bit in the bit field.
                    Range 0..7.
  @param  AndData   The value to AND with read value from the MMIO register.

  @return The value written back to the MMIO register.

**/
UINT8
EFIAPI
MmioBitFieldAnd8 (
  IN      UINTN  Address,
  IN      UINTN  StartBit,
  IN      UINTN  EndBit,
  IN      UINT8  AndData
  )
{
  return 0;
}

/**
  Reads a bit field in an 8-bit MMIO register, performs a bitwise AND followed
  by a bitwise OR, and writes the result back to the bit field in the
  8-bit MMIO register.

  Reads the 8-bit MMIO register specified by Address, performs a bitwise AND
  followed by a bitwise OR between the read result and the value
  specified by AndData, and writes the result to the 8-bit MMIO register
  specified by Address. The value written to the MMIO register is returned.
  This function must guarantee that all MMIO read and write operations are
  serialized. Extra left bits in both AndData and OrData are stripped.

  If 8-bit MMIO register operations are not supported, then ASSERT().
  If StartBit is greater than 7, then ASSERT().
  If EndBit is greater than 7, then ASSERT().
  If EndBit is less than StartBit, then ASSERT().
  If AndData is larger than the bitmask value range specified by StartBit and EndBit, then ASSERT().
  If OrData is larger than the bitmask value range specified by StartBit and EndBit, then ASSERT().

  @param  Address   MMIO register to write.
  @param  StartBit  The ordinal of the least significant bit in the bit field.
                    Range 0..7.
  @param  EndBit    The ordinal of the most significant bit in the bit field.
                    Range 0..7.
  @param  AndData   The value to AND with read value from the MMIO register.
  @param  OrData    The value to OR with the result of the AND operation.

  @return The value written back to the MMIO register.

**/
UINT8
EFIAPI
MmioBitFieldAndThenOr8 (
  IN      UINTN  Address,
  IN      UINTN  StartBit,
  IN      UINTN  EndBit,
  IN      UINT8  AndData,
  IN      UINT8  OrData
  )
{
  return 0;
}

/**
  Reads a 16-bit MMIO register.

  Reads the 16-bit MMIO register specified by Address. The 16-bit read value is
  returned. This function must guarantee that all MMIO read and write
  operations are serialized.

  If 16-bit MMIO register operations are not supported, then ASSERT().
  If Address is not aligned on a 16-bit boundary, then ASSERT().

  @param  Address The MMIO register to read.

  @return The value read.

**/
UINT16
EFIAPI
MmioRead16 (
  IN      UINTN  Address
  )
{
  REGISTER_SPACE_MOCK  *Mock;
  UINT64               Value;
  UINT64               Offset;

  Mock = MockIoGetRegisterSpace (Address, MockIoTypeMmio, &Offset);
  if (Mock == NULL) {
    return 0xFFFF;
  }

  Mock->Read (Mock, Offset, 2, &Value);
  return (UINT16) Value;
}

/**
  Writes a 16-bit MMIO register.

  Writes the 16-bit MMIO register specified by Address with the value specified
  by Value and returns Value. This function must guarantee that all MMIO read
  and write operations are serialized.

  If 16-bit MMIO register operations are not supported, then ASSERT().
  If Address is not aligned on a 16-bit boundary, then ASSERT().

  @param  Address The MMIO register to write.
  @param  Value   The value to write to the MMIO register.

  @return Value.

**/
UINT16
EFIAPI
MmioWrite16 (
  IN      UINTN   Address,
  IN      UINT16  Value
  )
{
  REGISTER_SPACE_MOCK  *Mock;
  UINT64               Val;
  UINT64               Offset;

  Mock = MockIoGetRegisterSpace (Address, MockIoTypeMmio, &Offset);
  if (Mock == NULL) {
    return 0xFFFF;
  }

  Val = Value;
  Mock->Write (Mock, Offset, 2, Val);
  return (UINT16) Value;
}

/**
  Reads a 16-bit MMIO register, performs a bitwise OR, and writes the
  result back to the 16-bit MMIO register.

  Reads the 16-bit MMIO register specified by Address, performs a bitwise
  OR between the read result and the value specified by OrData, and
  writes the result to the 16-bit MMIO register specified by Address. The value
  written to the MMIO register is returned. This function must guarantee that
  all MMIO read and write operations are serialized.

  If 16-bit MMIO register operations are not supported, then ASSERT().
  If Address is not aligned on a 16-bit boundary, then ASSERT().

  @param  Address The MMIO register to write.
  @param  OrData  The value to OR with the read value from the MMIO register.

  @return The value written back to the MMIO register.

**/
UINT16
EFIAPI
MmioOr16 (
  IN      UINTN   Address,
  IN      UINT16  OrData
  )
{
  UINT16  Val;

  Val = MmioRead16 (Address);
  Val |= OrData;
  return MmioWrite16 (Address, Val);
}

/**
  Reads a 16-bit MMIO register, performs a bitwise AND, and writes the result
  back to the 16-bit MMIO register.

  Reads the 16-bit MMIO register specified by Address, performs a bitwise AND
  between the read result and the value specified by AndData, and writes the
  result to the 16-bit MMIO register specified by Address. The value written to
  the MMIO register is returned. This function must guarantee that all MMIO
  read and write operations are serialized.

  If 16-bit MMIO register operations are not supported, then ASSERT().
  If Address is not aligned on a 16-bit boundary, then ASSERT().

  @param  Address The MMIO register to write.
  @param  AndData The value to AND with the read value from the MMIO register.

  @return The value written back to the MMIO register.

**/
UINT16
EFIAPI
MmioAnd16 (
  IN      UINTN   Address,
  IN      UINT16  AndData
  )
{
  UINT16  Val;

  Val = MmioRead16 (Address);
  Val &= AndData;
  return MmioWrite16 (Address, Val);
}

/**
  Reads a 16-bit MMIO register, performs a bitwise AND followed by a bitwise
  OR, and writes the result back to the 16-bit MMIO register.

  Reads the 16-bit MMIO register specified by Address, performs a bitwise AND
  between the read result and the value specified by AndData, performs a
  bitwise OR between the result of the AND operation and the value specified by
  OrData, and writes the result to the 16-bit MMIO register specified by
  Address. The value written to the MMIO register is returned. This function
  must guarantee that all MMIO read and write operations are serialized.

  If 16-bit MMIO register operations are not supported, then ASSERT().
  If Address is not aligned on a 16-bit boundary, then ASSERT().

  @param  Address The MMIO register to write.
  @param  AndData The value to AND with the read value from the MMIO register.
  @param  OrData  The value to OR with the result of the AND operation.

  @return The value written back to the MMIO register.

**/
UINT16
EFIAPI
MmioAndThenOr16 (
  IN      UINTN   Address,
  IN      UINT16  AndData,
  IN      UINT16  OrData
  )
{
  UINT16  Val;

  Val = MmioRead16 (Address);
  Val &= AndData;
  Val |= OrData;
  return MmioWrite16 (Address, Val);
}

/**
  Reads a bit field of a MMIO register.

  Reads the bit field in a 16-bit MMIO register. The bit field is specified by
  the StartBit and the EndBit. The value of the bit field is returned.

  If 16-bit MMIO register operations are not supported, then ASSERT().
  If Address is not aligned on a 16-bit boundary, then ASSERT().
  If StartBit is greater than 15, then ASSERT().
  If EndBit is greater than 15, then ASSERT().
  If EndBit is less than StartBit, then ASSERT().

  @param  Address   MMIO register to read.
  @param  StartBit  The ordinal of the least significant bit in the bit field.
                    Range 0..15.
  @param  EndBit    The ordinal of the most significant bit in the bit field.
                    Range 0..15.

  @return The value read.

**/
UINT16
EFIAPI
MmioBitFieldRead16 (
  IN      UINTN  Address,
  IN      UINTN  StartBit,
  IN      UINTN  EndBit
  )
{
  return 0;
}

/**
  Writes a bit field to a MMIO register.

  Writes Value to the bit field of the MMIO register. The bit field is
  specified by the StartBit and the EndBit. All other bits in the destination
  MMIO register are preserved. The new value of the 16-bit register is returned.

  If 16-bit MMIO register operations are not supported, then ASSERT().
  If Address is not aligned on a 16-bit boundary, then ASSERT().
  If StartBit is greater than 15, then ASSERT().
  If EndBit is greater than 15, then ASSERT().
  If EndBit is less than StartBit, then ASSERT().
  If Value is larger than the bitmask value range specified by StartBit and EndBit, then ASSERT().

  @param  Address   MMIO register to write.
  @param  StartBit  The ordinal of the least significant bit in the bit field.
                    Range 0..15.
  @param  EndBit    The ordinal of the most significant bit in the bit field.
                    Range 0..15.
  @param  Value     New value of the bit field.

  @return The value written back to the MMIO register.

**/
UINT16
EFIAPI
MmioBitFieldWrite16 (
  IN      UINTN   Address,
  IN      UINTN   StartBit,
  IN      UINTN   EndBit,
  IN      UINT16  Value
  )
{
  return 0;
}

/**
  Reads a bit field in a 16-bit MMIO register, performs a bitwise OR, and
  writes the result back to the bit field in the 16-bit MMIO register.

  Reads the 16-bit MMIO register specified by Address, performs a bitwise
  OR between the read result and the value specified by OrData, and
  writes the result to the 16-bit MMIO register specified by Address. The value
  written to the MMIO register is returned. This function must guarantee that
  all MMIO read and write operations are serialized. Extra left bits in OrData
  are stripped.

  If 16-bit MMIO register operations are not supported, then ASSERT().
  If Address is not aligned on a 16-bit boundary, then ASSERT().
  If StartBit is greater than 15, then ASSERT().
  If EndBit is greater than 15, then ASSERT().
  If EndBit is less than StartBit, then ASSERT().
  If OrData is larger than the bitmask value range specified by StartBit and EndBit, then ASSERT().

  @param  Address   MMIO register to write.
  @param  StartBit  The ordinal of the least significant bit in the bit field.
                    Range 0..15.
  @param  EndBit    The ordinal of the most significant bit in the bit field.
                    Range 0..15.
  @param  OrData    The value to OR with read value from the MMIO register.

  @return The value written back to the MMIO register.

**/
UINT16
EFIAPI
MmioBitFieldOr16 (
  IN      UINTN   Address,
  IN      UINTN   StartBit,
  IN      UINTN   EndBit,
  IN      UINT16  OrData
  )
{
  return 0;
}

/**
  Reads a bit field in a 16-bit MMIO register, performs a bitwise AND, and
  writes the result back to the bit field in the 16-bit MMIO register.

  Reads the 16-bit MMIO register specified by Address, performs a bitwise AND
  between the read result and the value specified by AndData, and writes the
  result to the 16-bit MMIO register specified by Address. The value written to
  the MMIO register is returned. This function must guarantee that all MMIO
  read and write operations are serialized. Extra left bits in AndData are
  stripped.

  If 16-bit MMIO register operations are not supported, then ASSERT().
  If Address is not aligned on a 16-bit boundary, then ASSERT().
  If StartBit is greater than 15, then ASSERT().
  If EndBit is greater than 15, then ASSERT().
  If EndBit is less than StartBit, then ASSERT().
  If AndData is larger than the bitmask value range specified by StartBit and EndBit, then ASSERT().

  @param  Address   MMIO register to write.
  @param  StartBit  The ordinal of the least significant bit in the bit field.
                    Range 0..15.
  @param  EndBit    The ordinal of the most significant bit in the bit field.
                    Range 0..15.
  @param  AndData   The value to AND with read value from the MMIO register.

  @return The value written back to the MMIO register.

**/
UINT16
EFIAPI
MmioBitFieldAnd16 (
  IN      UINTN   Address,
  IN      UINTN   StartBit,
  IN      UINTN   EndBit,
  IN      UINT16  AndData
  )
{
  return 0;
}

/**
  Reads a bit field in a 16-bit MMIO register, performs a bitwise AND followed
  by a bitwise OR, and writes the result back to the bit field in the
  16-bit MMIO register.

  Reads the 16-bit MMIO register specified by Address, performs a bitwise AND
  followed by a bitwise OR between the read result and the value
  specified by AndData, and writes the result to the 16-bit MMIO register
  specified by Address. The value written to the MMIO register is returned.
  This function must guarantee that all MMIO read and write operations are
  serialized. Extra left bits in both AndData and OrData are stripped.

  If 16-bit MMIO register operations are not supported, then ASSERT().
  If Address is not aligned on a 16-bit boundary, then ASSERT().
  If StartBit is greater than 15, then ASSERT().
  If EndBit is greater than 15, then ASSERT().
  If EndBit is less than StartBit, then ASSERT().
  If AndData is larger than the bitmask value range specified by StartBit and EndBit, then ASSERT().
  If OrData is larger than the bitmask value range specified by StartBit and EndBit, then ASSERT().

  @param  Address   MMIO register to write.
  @param  StartBit  The ordinal of the least significant bit in the bit field.
                    Range 0..15.
  @param  EndBit    The ordinal of the most significant bit in the bit field.
                    Range 0..15.
  @param  AndData   The value to AND with read value from the MMIO register.
  @param  OrData    The value to OR with the result of the AND operation.

  @return The value written back to the MMIO register.

**/
UINT16
EFIAPI
MmioBitFieldAndThenOr16 (
  IN      UINTN   Address,
  IN      UINTN   StartBit,
  IN      UINTN   EndBit,
  IN      UINT16  AndData,
  IN      UINT16  OrData
  )
{
  return 0;
}

/**
  Reads a 32-bit MMIO register.

  Reads the 32-bit MMIO register specified by Address. The 32-bit read value is
  returned. This function must guarantee that all MMIO read and write
  operations are serialized.

  If 32-bit MMIO register operations are not supported, then ASSERT().
  If Address is not aligned on a 32-bit boundary, then ASSERT().

  @param  Address The MMIO register to read.

  @return The value read.

**/
UINT32
EFIAPI
MmioRead32 (
  IN      UINTN  Address
  )
{
  REGISTER_SPACE_MOCK  *Mock;
  UINT64               Value;
  UINT64               Offset;

  Mock = MockIoGetRegisterSpace (Address, MockIoTypeMmio, &Offset);
  if (Mock == NULL) {
    return 0xFFFFFFFF;
  }

  Mock->Read (Mock, Offset, 4, &Value);
  return (UINT32) Value;
}

/**
  Writes a 32-bit MMIO register.

  Writes the 32-bit MMIO register specified by Address with the value specified
  by Value and returns Value. This function must guarantee that all MMIO read
  and write operations are serialized.

  If 32-bit MMIO register operations are not supported, then ASSERT().
  If Address is not aligned on a 32-bit boundary, then ASSERT().

  @param  Address The MMIO register to write.
  @param  Value   The value to write to the MMIO register.

  @return Value.

**/
UINT32
EFIAPI
MmioWrite32 (
  IN      UINTN   Address,
  IN      UINT32  Value
  )
{
  REGISTER_SPACE_MOCK  *Mock;
  UINT64               Val;
  UINT64               Offset;

  Mock = MockIoGetRegisterSpace (Address, MockIoTypeMmio, &Offset);
  if (Mock == NULL) {
    return 0xFFFFFFFF;
  }

  Val = Value;
  Mock->Write (Mock, Offset, 4, Val);
  return (UINT32) Value;
}

/**
  Reads a 32-bit MMIO register, performs a bitwise OR, and writes the
  result back to the 32-bit MMIO register.

  Reads the 32-bit MMIO register specified by Address, performs a bitwise
  OR between the read result and the value specified by OrData, and
  writes the result to the 32-bit MMIO register specified by Address. The value
  written to the MMIO register is returned. This function must guarantee that
  all MMIO read and write operations are serialized.

  If 32-bit MMIO register operations are not supported, then ASSERT().
  If Address is not aligned on a 32-bit boundary, then ASSERT().

  @param  Address The MMIO register to write.
  @param  OrData  The value to OR with the read value from the MMIO register.

  @return The value written back to the MMIO register.

**/
UINT32
EFIAPI
MmioOr32 (
  IN      UINTN   Address,
  IN      UINT32  OrData
  )
{
  UINT32  Val;

  Val = MmioRead32 (Address);
  Val |= OrData;
  return MmioWrite32 (Address, Val);
}

/**
  Reads a 32-bit MMIO register, performs a bitwise AND, and writes the result
  back to the 32-bit MMIO register.

  Reads the 32-bit MMIO register specified by Address, performs a bitwise AND
  between the read result and the value specified by AndData, and writes the
  result to the 32-bit MMIO register specified by Address. The value written to
  the MMIO register is returned. This function must guarantee that all MMIO
  read and write operations are serialized.

  If 32-bit MMIO register operations are not supported, then ASSERT().
  If Address is not aligned on a 32-bit boundary, then ASSERT().

  @param  Address The MMIO register to write.
  @param  AndData The value to AND with the read value from the MMIO register.

  @return The value written back to the MMIO register.

**/
UINT32
EFIAPI
MmioAnd32 (
  IN      UINTN   Address,
  IN      UINT32  AndData
  )
{
  UINT32  Val;

  Val = MmioRead32 (Address);
  Val &= AndData;
  return MmioWrite32 (Address, Val);
}

/**
  Reads a 32-bit MMIO register, performs a bitwise AND followed by a bitwise
  OR, and writes the result back to the 32-bit MMIO register.

  Reads the 32-bit MMIO register specified by Address, performs a bitwise AND
  between the read result and the value specified by AndData, performs a
  bitwise OR between the result of the AND operation and the value specified by
  OrData, and writes the result to the 32-bit MMIO register specified by
  Address. The value written to the MMIO register is returned. This function
  must guarantee that all MMIO read and write operations are serialized.

  If 32-bit MMIO register operations are not supported, then ASSERT().
  If Address is not aligned on a 32-bit boundary, then ASSERT().

  @param  Address The MMIO register to write.
  @param  AndData The value to AND with the read value from the MMIO register.
  @param  OrData  The value to OR with the result of the AND operation.

  @return The value written back to the MMIO register.

**/
UINT32
EFIAPI
MmioAndThenOr32 (
  IN      UINTN   Address,
  IN      UINT32  AndData,
  IN      UINT32  OrData
  )
{
  UINT32  Val;

  Val = MmioRead32 (Address);
  Val &= AndData;
  Val |= OrData;
  return MmioWrite32 (Address, Val);
}

/**
  Reads a bit field of a MMIO register.

  Reads the bit field in a 32-bit MMIO register. The bit field is specified by
  the StartBit and the EndBit. The value of the bit field is returned.

  If 32-bit MMIO register operations are not supported, then ASSERT().
  If Address is not aligned on a 32-bit boundary, then ASSERT().
  If StartBit is greater than 31, then ASSERT().
  If EndBit is greater than 31, then ASSERT().
  If EndBit is less than StartBit, then ASSERT().

  @param  Address   MMIO register to read.
  @param  StartBit  The ordinal of the least significant bit in the bit field.
                    Range 0..31.
  @param  EndBit    The ordinal of the most significant bit in the bit field.
                    Range 0..31.

  @return The value read.

**/
UINT32
EFIAPI
MmioBitFieldRead32 (
  IN      UINTN  Address,
  IN      UINTN  StartBit,
  IN      UINTN  EndBit
  )
{
  return 0;
}

/**
  Writes a bit field to a MMIO register.

  Writes Value to the bit field of the MMIO register. The bit field is
  specified by the StartBit and the EndBit. All other bits in the destination
  MMIO register are preserved. The new value of the 32-bit register is returned.

  If 32-bit MMIO register operations are not supported, then ASSERT().
  If Address is not aligned on a 32-bit boundary, then ASSERT().
  If StartBit is greater than 31, then ASSERT().
  If EndBit is greater than 31, then ASSERT().
  If EndBit is less than StartBit, then ASSERT().
  If Value is larger than the bitmask value range specified by StartBit and EndBit, then ASSERT().

  @param  Address   MMIO register to write.
  @param  StartBit  The ordinal of the least significant bit in the bit field.
                    Range 0..31.
  @param  EndBit    The ordinal of the most significant bit in the bit field.
                    Range 0..31.
  @param  Value     New value of the bit field.

  @return The value written back to the MMIO register.

**/
UINT32
EFIAPI
MmioBitFieldWrite32 (
  IN      UINTN   Address,
  IN      UINTN   StartBit,
  IN      UINTN   EndBit,
  IN      UINT32  Value
  )
{
  return 0;
}

/**
  Reads a bit field in a 32-bit MMIO register, performs a bitwise OR, and
  writes the result back to the bit field in the 32-bit MMIO register.

  Reads the 32-bit MMIO register specified by Address, performs a bitwise
  OR between the read result and the value specified by OrData, and
  writes the result to the 32-bit MMIO register specified by Address. The value
  written to the MMIO register is returned. This function must guarantee that
  all MMIO read and write operations are serialized. Extra left bits in OrData
  are stripped.

  If 32-bit MMIO register operations are not supported, then ASSERT().
  If Address is not aligned on a 32-bit boundary, then ASSERT().
  If StartBit is greater than 31, then ASSERT().
  If EndBit is greater than 31, then ASSERT().
  If EndBit is less than StartBit, then ASSERT().
  If OrData is larger than the bitmask value range specified by StartBit and EndBit, then ASSERT().

  @param  Address   MMIO register to write.
  @param  StartBit  The ordinal of the least significant bit in the bit field.
                    Range 0..31.
  @param  EndBit    The ordinal of the most significant bit in the bit field.
                    Range 0..31.
  @param  OrData    The value to OR with read value from the MMIO register.

  @return The value written back to the MMIO register.

**/
UINT32
EFIAPI
MmioBitFieldOr32 (
  IN      UINTN   Address,
  IN      UINTN   StartBit,
  IN      UINTN   EndBit,
  IN      UINT32  OrData
  )
{
  return 0;
}

/**
  Reads a bit field in a 32-bit MMIO register, performs a bitwise AND, and
  writes the result back to the bit field in the 32-bit MMIO register.

  Reads the 32-bit MMIO register specified by Address, performs a bitwise AND
  between the read result and the value specified by AndData, and writes the
  result to the 32-bit MMIO register specified by Address. The value written to
  the MMIO register is returned. This function must guarantee that all MMIO
  read and write operations are serialized. Extra left bits in AndData are
  stripped.

  If 32-bit MMIO register operations are not supported, then ASSERT().
  If Address is not aligned on a 32-bit boundary, then ASSERT().
  If StartBit is greater than 31, then ASSERT().
  If EndBit is greater than 31, then ASSERT().
  If EndBit is less than StartBit, then ASSERT().
  If AndData is larger than the bitmask value range specified by StartBit and EndBit, then ASSERT().

  @param  Address   MMIO register to write.
  @param  StartBit  The ordinal of the least significant bit in the bit field.
                    Range 0..31.
  @param  EndBit    The ordinal of the most significant bit in the bit field.
                    Range 0..31.
  @param  AndData   The value to AND with read value from the MMIO register.

  @return The value written back to the MMIO register.

**/
UINT32
EFIAPI
MmioBitFieldAnd32 (
  IN      UINTN   Address,
  IN      UINTN   StartBit,
  IN      UINTN   EndBit,
  IN      UINT32  AndData
  )
{
  return 0;
}

/**
  Reads a bit field in a 32-bit MMIO register, performs a bitwise AND followed
  by a bitwise OR, and writes the result back to the bit field in the
  32-bit MMIO register.

  Reads the 32-bit MMIO register specified by Address, performs a bitwise AND
  followed by a bitwise OR between the read result and the value
  specified by AndData, and writes the result to the 32-bit MMIO register
  specified by Address. The value written to the MMIO register is returned.
  This function must guarantee that all MMIO read and write operations are
  serialized. Extra left bits in both AndData and OrData are stripped.

  If 32-bit MMIO register operations are not supported, then ASSERT().
  If Address is not aligned on a 32-bit boundary, then ASSERT().
  If StartBit is greater than 31, then ASSERT().
  If EndBit is greater than 31, then ASSERT().
  If EndBit is less than StartBit, then ASSERT().
  If AndData is larger than the bitmask value range specified by StartBit and EndBit, then ASSERT().
  If OrData is larger than the bitmask value range specified by StartBit and EndBit, then ASSERT().

  @param  Address   MMIO register to write.
  @param  StartBit  The ordinal of the least significant bit in the bit field.
                    Range 0..31.
  @param  EndBit    The ordinal of the most significant bit in the bit field.
                    Range 0..31.
  @param  AndData   The value to AND with read value from the MMIO register.
  @param  OrData    The value to OR with the result of the AND operation.

  @return The value written back to the MMIO register.

**/
UINT32
EFIAPI
MmioBitFieldAndThenOr32 (
  IN      UINTN   Address,
  IN      UINTN   StartBit,
  IN      UINTN   EndBit,
  IN      UINT32  AndData,
  IN      UINT32  OrData
  )
{
  return 0;
}

/**
  Reads a 64-bit MMIO register.

  Reads the 64-bit MMIO register specified by Address. The 64-bit read value is
  returned. This function must guarantee that all MMIO read and write
  operations are serialized.

  If 64-bit MMIO register operations are not supported, then ASSERT().
  If Address is not aligned on a 64-bit boundary, then ASSERT().

  @param  Address The MMIO register to read.

  @return The value read.

**/
UINT64
EFIAPI
MmioRead64 (
  IN      UINTN  Address
  )
{
  REGISTER_SPACE_MOCK  *Mock;
  UINT64               Value;
  UINT64               Offset;

  Mock = MockIoGetRegisterSpace (Address, MockIoTypeMmio, &Offset);
  if (Mock == NULL) {
    return 0xFFFFFFFFFFFFFFFF;
  }

  Mock->Read (Mock, Offset, 8, &Value);
  return Value;
}

/**
  Writes a 64-bit MMIO register.

  Writes the 64-bit MMIO register specified by Address with the value specified
  by Value and returns Value. This function must guarantee that all MMIO read
  and write operations are serialized.

  If 64-bit MMIO register operations are not supported, then ASSERT().
  If Address is not aligned on a 64-bit boundary, then ASSERT().

  @param  Address The MMIO register to write.
  @param  Value   The value to write to the MMIO register.

**/
UINT64
EFIAPI
MmioWrite64 (
  IN      UINTN   Address,
  IN      UINT64  Value
  )
{
  REGISTER_SPACE_MOCK  *Mock;
  UINT64               Val;
  UINT64               Offset;

  Mock = MockIoGetRegisterSpace (Address, MockIoTypeMmio, &Offset);
  if (Mock == NULL) {
    return 0xFFFFFFFFFFFFFFFF;
  }

  Val = Value;
  Mock->Write (Mock, Offset, 8, Val);
  return Value;
}

/**
  Reads a 64-bit MMIO register, performs a bitwise OR, and writes the
  result back to the 64-bit MMIO register.

  Reads the 64-bit MMIO register specified by Address, performs a bitwise
  OR between the read result and the value specified by OrData, and
  writes the result to the 64-bit MMIO register specified by Address. The value
  written to the MMIO register is returned. This function must guarantee that
  all MMIO read and write operations are serialized.

  If 64-bit MMIO register operations are not supported, then ASSERT().
  If Address is not aligned on a 64-bit boundary, then ASSERT().

  @param  Address The MMIO register to write.
  @param  OrData  The value to OR with the read value from the MMIO register.

  @return The value written back to the MMIO register.

**/
UINT64
EFIAPI
MmioOr64 (
  IN      UINTN   Address,
  IN      UINT64  OrData
  )
{
  UINT64  Val;

  Val = MmioRead64 (Address);
  Val |= OrData;
  return MmioWrite64 (Address, Val);
}

/**
  Reads a 64-bit MMIO register, performs a bitwise AND, and writes the result
  back to the 64-bit MMIO register.

  Reads the 64-bit MMIO register specified by Address, performs a bitwise AND
  between the read result and the value specified by AndData, and writes the
  result to the 64-bit MMIO register specified by Address. The value written to
  the MMIO register is returned. This function must guarantee that all MMIO
  read and write operations are serialized.

  If 64-bit MMIO register operations are not supported, then ASSERT().
  If Address is not aligned on a 64-bit boundary, then ASSERT().

  @param  Address The MMIO register to write.
  @param  AndData The value to AND with the read value from the MMIO register.

  @return The value written back to the MMIO register.

**/
UINT64
EFIAPI
MmioAnd64 (
  IN      UINTN   Address,
  IN      UINT64  AndData
  )
{
  UINT64  Val;

  Val = MmioRead64 (Address);
  Val &= AndData;
  return MmioWrite64 (Address, Val);
}

/**
  Reads a 64-bit MMIO register, performs a bitwise AND followed by a bitwise
  OR, and writes the result back to the 64-bit MMIO register.

  Reads the 64-bit MMIO register specified by Address, performs a bitwise AND
  between the read result and the value specified by AndData, performs a
  bitwise OR between the result of the AND operation and the value specified by
  OrData, and writes the result to the 64-bit MMIO register specified by
  Address. The value written to the MMIO register is returned. This function
  must guarantee that all MMIO read and write operations are serialized.

  If 64-bit MMIO register operations are not supported, then ASSERT().
  If Address is not aligned on a 64-bit boundary, then ASSERT().

  @param  Address The MMIO register to write.
  @param  AndData The value to AND with the read value from the MMIO register.
  @param  OrData  The value to OR with the result of the AND operation.

  @return The value written back to the MMIO register.

**/
UINT64
EFIAPI
MmioAndThenOr64 (
  IN      UINTN   Address,
  IN      UINT64  AndData,
  IN      UINT64  OrData
  )
{
  UINT64  Val;

  Val = MmioRead64 (Address);
  Val &= AndData;
  Val |= OrData;
  return MmioWrite64 (Address, Val);
}

/**
  Reads a bit field of a MMIO register.

  Reads the bit field in a 64-bit MMIO register. The bit field is specified by
  the StartBit and the EndBit. The value of the bit field is returned.

  If 64-bit MMIO register operations are not supported, then ASSERT().
  If Address is not aligned on a 64-bit boundary, then ASSERT().
  If StartBit is greater than 63, then ASSERT().
  If EndBit is greater than 63, then ASSERT().
  If EndBit is less than StartBit, then ASSERT().

  @param  Address   MMIO register to read.
  @param  StartBit  The ordinal of the least significant bit in the bit field.
                    Range 0..63.
  @param  EndBit    The ordinal of the most significant bit in the bit field.
                    Range 0..63.

  @return The value read.

**/
UINT64
EFIAPI
MmioBitFieldRead64 (
  IN      UINTN  Address,
  IN      UINTN  StartBit,
  IN      UINTN  EndBit
  )
{
  return 0;
}

/**
  Writes a bit field to a MMIO register.

  Writes Value to the bit field of the MMIO register. The bit field is
  specified by the StartBit and the EndBit. All other bits in the destination
  MMIO register are preserved. The new value of the 64-bit register is returned.

  If 64-bit MMIO register operations are not supported, then ASSERT().
  If Address is not aligned on a 64-bit boundary, then ASSERT().
  If StartBit is greater than 63, then ASSERT().
  If EndBit is greater than 63, then ASSERT().
  If EndBit is less than StartBit, then ASSERT().
  If Value is larger than the bitmask value range specified by StartBit and EndBit, then ASSERT().

  @param  Address   MMIO register to write.
  @param  StartBit  The ordinal of the least significant bit in the bit field.
                    Range 0..63.
  @param  EndBit    The ordinal of the most significant bit in the bit field.
                    Range 0..63.
  @param  Value     New value of the bit field.

  @return The value written back to the MMIO register.

**/
UINT64
EFIAPI
MmioBitFieldWrite64 (
  IN      UINTN   Address,
  IN      UINTN   StartBit,
  IN      UINTN   EndBit,
  IN      UINT64  Value
  )
{
  return 0;
}

/**
  Reads a bit field in a 64-bit MMIO register, performs a bitwise OR, and
  writes the result back to the bit field in the 64-bit MMIO register.

  Reads the 64-bit MMIO register specified by Address, performs a bitwise
  OR between the read result and the value specified by OrData, and
  writes the result to the 64-bit MMIO register specified by Address. The value
  written to the MMIO register is returned. This function must guarantee that
  all MMIO read and write operations are serialized. Extra left bits in OrData
  are stripped.

  If 64-bit MMIO register operations are not supported, then ASSERT().
  If Address is not aligned on a 64-bit boundary, then ASSERT().
  If StartBit is greater than 63, then ASSERT().
  If EndBit is greater than 63, then ASSERT().
  If EndBit is less than StartBit, then ASSERT().
  If OrData is larger than the bitmask value range specified by StartBit and EndBit, then ASSERT().

  @param  Address   MMIO register to write.
  @param  StartBit  The ordinal of the least significant bit in the bit field.
                    Range 0..63.
  @param  EndBit    The ordinal of the most significant bit in the bit field.
                    Range 0..63.
  @param  OrData    The value to OR with read value from the MMIO register.

  @return The value written back to the MMIO register.

**/
UINT64
EFIAPI
MmioBitFieldOr64 (
  IN      UINTN   Address,
  IN      UINTN   StartBit,
  IN      UINTN   EndBit,
  IN      UINT64  OrData
  )
{
  return 0;
}

/**
  Reads a bit field in a 64-bit MMIO register, performs a bitwise AND, and
  writes the result back to the bit field in the 64-bit MMIO register.

  Reads the 64-bit MMIO register specified by Address, performs a bitwise AND
  between the read result and the value specified by AndData, and writes the
  result to the 64-bit MMIO register specified by Address. The value written to
  the MMIO register is returned. This function must guarantee that all MMIO
  read and write operations are serialized. Extra left bits in AndData are
  stripped.

  If 64-bit MMIO register operations are not supported, then ASSERT().
  If Address is not aligned on a 64-bit boundary, then ASSERT().
  If StartBit is greater than 63, then ASSERT().
  If EndBit is greater than 63, then ASSERT().
  If EndBit is less than StartBit, then ASSERT().
  If AndData is larger than the bitmask value range specified by StartBit and EndBit, then ASSERT().

  @param  Address   MMIO register to write.
  @param  StartBit  The ordinal of the least significant bit in the bit field.
                    Range 0..63.
  @param  EndBit    The ordinal of the most significant bit in the bit field.
                    Range 0..63.
  @param  AndData   The value to AND with read value from the MMIO register.

  @return The value written back to the MMIO register.

**/
UINT64
EFIAPI
MmioBitFieldAnd64 (
  IN      UINTN   Address,
  IN      UINTN   StartBit,
  IN      UINTN   EndBit,
  IN      UINT64  AndData
  )
{
  return 0;
}

/**
  Reads a bit field in a 64-bit MMIO register, performs a bitwise AND followed
  by a bitwise OR, and writes the result back to the bit field in the
  64-bit MMIO register.

  Reads the 64-bit MMIO register specified by Address, performs a bitwise AND
  followed by a bitwise OR between the read result and the value
  specified by AndData, and writes the result to the 64-bit MMIO register
  specified by Address. The value written to the MMIO register is returned.
  This function must guarantee that all MMIO read and write operations are
  serialized. Extra left bits in both AndData and OrData are stripped.

  If 64-bit MMIO register operations are not supported, then ASSERT().
  If Address is not aligned on a 64-bit boundary, then ASSERT().
  If StartBit is greater than 63, then ASSERT().
  If EndBit is greater than 63, then ASSERT().
  If EndBit is less than StartBit, then ASSERT().
  If AndData is larger than the bitmask value range specified by StartBit and EndBit, then ASSERT().
  If OrData is larger than the bitmask value range specified by StartBit and EndBit, then ASSERT().

  @param  Address   MMIO register to write.
  @param  StartBit  The ordinal of the least significant bit in the bit field.
                    Range 0..63.
  @param  EndBit    The ordinal of the most significant bit in the bit field.
                    Range 0..63.
  @param  AndData   The value to AND with read value from the MMIO register.
  @param  OrData    The value to OR with the result of the AND operation.

  @return The value written back to the MMIO register.

**/
UINT64
EFIAPI
MmioBitFieldAndThenOr64 (
  IN      UINTN   Address,
  IN      UINTN   StartBit,
  IN      UINTN   EndBit,
  IN      UINT64  AndData,
  IN      UINT64  OrData
  )
{
  return 0;
}

/**
  Copy data from MMIO region to system memory by using 8-bit access.

  Copy data from MMIO region specified by starting address StartAddress
  to system memory specified by Buffer by using 8-bit access. The total
  number of byte to be copied is specified by Length. Buffer is returned.

  If Length is greater than (MAX_ADDRESS - StartAddress + 1), then ASSERT().
  If Length is greater than (MAX_ADDRESS - Buffer + 1), then ASSERT().


  @param  StartAddress    Starting address for the MMIO region to be copied from.
  @param  Length          The size, in bytes, of Buffer.
  @param  Buffer          Pointer to a system memory buffer receiving the data read.

  @return Buffer

**/
UINT8 *
EFIAPI
MmioReadBuffer8 (
  IN  UINTN  StartAddress,
  IN  UINTN  Length,
  OUT UINT8  *Buffer
  )
{
  return 0;
}

/**
  Copy data from MMIO region to system memory by using 16-bit access.

  Copy data from MMIO region specified by starting address StartAddress
  to system memory specified by Buffer by using 16-bit access. The total
  number of byte to be copied is specified by Length. Buffer is returned.

  If StartAddress is not aligned on a 16-bit boundary, then ASSERT().

  If Length is greater than (MAX_ADDRESS - StartAddress + 1), then ASSERT().
  If Length is greater than (MAX_ADDRESS - Buffer + 1), then ASSERT().

  If Length is not aligned on a 16-bit boundary, then ASSERT().
  If Buffer is not aligned on a 16-bit boundary, then ASSERT().

  @param  StartAddress    Starting address for the MMIO region to be copied from.
  @param  Length          The size, in bytes, of Buffer.
  @param  Buffer          Pointer to a system memory buffer receiving the data read.

  @return Buffer

**/
UINT16 *
EFIAPI
MmioReadBuffer16 (
  IN  UINTN   StartAddress,
  IN  UINTN   Length,
  OUT UINT16  *Buffer
  )
{
  return 0;
}

/**
  Copy data from MMIO region to system memory by using 32-bit access.

  Copy data from MMIO region specified by starting address StartAddress
  to system memory specified by Buffer by using 32-bit access. The total
  number of byte to be copied is specified by Length. Buffer is returned.

  If StartAddress is not aligned on a 32-bit boundary, then ASSERT().

  If Length is greater than (MAX_ADDRESS - StartAddress + 1), then ASSERT().
  If Length is greater than (MAX_ADDRESS - Buffer + 1), then ASSERT().

  If Length is not aligned on a 32-bit boundary, then ASSERT().
  If Buffer is not aligned on a 32-bit boundary, then ASSERT().

  @param  StartAddress    Starting address for the MMIO region to be copied from.
  @param  Length          The size, in bytes, of Buffer.
  @param  Buffer          Pointer to a system memory buffer receiving the data read.

  @return Buffer

**/
UINT32 *
EFIAPI
MmioReadBuffer32 (
  IN  UINTN   StartAddress,
  IN  UINTN   Length,
  OUT UINT32  *Buffer
  )
{
  return 0;
}

/**
  Copy data from MMIO region to system memory by using 64-bit access.

  Copy data from MMIO region specified by starting address StartAddress
  to system memory specified by Buffer by using 64-bit access. The total
  number of byte to be copied is specified by Length. Buffer is returned.

  If StartAddress is not aligned on a 64-bit boundary, then ASSERT().

  If Length is greater than (MAX_ADDRESS - StartAddress + 1), then ASSERT().
  If Length is greater than (MAX_ADDRESS - Buffer + 1), then ASSERT().

  If Length is not aligned on a 64-bit boundary, then ASSERT().
  If Buffer is not aligned on a 64-bit boundary, then ASSERT().

  @param  StartAddress    Starting address for the MMIO region to be copied from.
  @param  Length          The size, in bytes, of Buffer.
  @param  Buffer          Pointer to a system memory buffer receiving the data read.

  @return Buffer

**/
UINT64 *
EFIAPI
MmioReadBuffer64 (
  IN  UINTN   StartAddress,
  IN  UINTN   Length,
  OUT UINT64  *Buffer
  )
{
  return 0;
}

/**
  Copy data from system memory to MMIO region by using 8-bit access.

  Copy data from system memory specified by Buffer to MMIO region specified
  by starting address StartAddress by using 8-bit access. The total number
  of byte to be copied is specified by Length. Buffer is returned.

  If Length is greater than (MAX_ADDRESS - StartAddress + 1), then ASSERT().
  If Length is greater than (MAX_ADDRESS -Buffer + 1), then ASSERT().


  @param  StartAddress    Starting address for the MMIO region to be copied to.
  @param  Length          The size, in bytes, of Buffer.
  @param  Buffer          Pointer to a system memory buffer containing the data to write.

  @return Buffer

**/
UINT8 *
EFIAPI
MmioWriteBuffer8 (
  IN  UINTN        StartAddress,
  IN  UINTN        Length,
  IN  CONST UINT8  *Buffer
  )
{
  return 0;
}

/**
  Copy data from system memory to MMIO region by using 16-bit access.

  Copy data from system memory specified by Buffer to MMIO region specified
  by starting address StartAddress by using 16-bit access. The total number
  of byte to be copied is specified by Length. Buffer is returned.

  If StartAddress is not aligned on a 16-bit boundary, then ASSERT().

  If Length is greater than (MAX_ADDRESS - StartAddress + 1), then ASSERT().
  If Length is greater than (MAX_ADDRESS -Buffer + 1), then ASSERT().

  If Length is not aligned on a 16-bit boundary, then ASSERT().

  If Buffer is not aligned on a 16-bit boundary, then ASSERT().

  @param  StartAddress    Starting address for the MMIO region to be copied to.
  @param  Length          The size, in bytes, of Buffer.
  @param  Buffer          Pointer to a system memory buffer containing the data to write.

  @return Buffer

**/
UINT16 *
EFIAPI
MmioWriteBuffer16 (
  IN  UINTN         StartAddress,
  IN  UINTN         Length,
  IN  CONST UINT16  *Buffer
  )
{
  return 0;
}

/**
  Copy data from system memory to MMIO region by using 32-bit access.

  Copy data from system memory specified by Buffer to MMIO region specified
  by starting address StartAddress by using 32-bit access. The total number
  of byte to be copied is specified by Length. Buffer is returned.

  If StartAddress is not aligned on a 32-bit boundary, then ASSERT().

  If Length is greater than (MAX_ADDRESS - StartAddress + 1), then ASSERT().
  If Length is greater than (MAX_ADDRESS -Buffer + 1), then ASSERT().

  If Length is not aligned on a 32-bit boundary, then ASSERT().

  If Buffer is not aligned on a 32-bit boundary, then ASSERT().

  @param  StartAddress    Starting address for the MMIO region to be copied to.
  @param  Length          The size, in bytes, of Buffer.
  @param  Buffer          Pointer to a system memory buffer containing the data to write.

  @return Buffer

**/
UINT32 *
EFIAPI
MmioWriteBuffer32 (
  IN  UINTN         StartAddress,
  IN  UINTN         Length,
  IN  CONST UINT32  *Buffer
  )
{
  return 0;
}

/**
  Copy data from system memory to MMIO region by using 64-bit access.

  Copy data from system memory specified by Buffer to MMIO region specified
  by starting address StartAddress by using 64-bit access. The total number
  of byte to be copied is specified by Length. Buffer is returned.

  If StartAddress is not aligned on a 64-bit boundary, then ASSERT().

  If Length is greater than (MAX_ADDRESS - StartAddress + 1), then ASSERT().
  If Length is greater than (MAX_ADDRESS -Buffer + 1), then ASSERT().

  If Length is not aligned on a 64-bit boundary, then ASSERT().

  If Buffer is not aligned on a 64-bit boundary, then ASSERT().

  @param  StartAddress    Starting address for the MMIO region to be copied to.
  @param  Length          The size, in bytes, of Buffer.
  @param  Buffer          Pointer to a system memory buffer containing the data to write.

  @return Buffer

**/
UINT64 *
EFIAPI
MmioWriteBuffer64 (
  IN  UINTN         StartAddress,
  IN  UINTN         Length,
  IN  CONST UINT64  *Buffer
  )
{
  return 0x0;
}