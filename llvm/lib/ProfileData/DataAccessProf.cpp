#include "llvm/ProfileData/DataAccessProf.h"
#include "llvm/ADT/STLExtras.h"

namespace llvm {

uint64_t
DataAccessProfData::addStringRef(StringRef Str,
                                 llvm::MapVector<StringRef, uint64_t> &Map) {
  auto [Iter, Inserted] = Map.insert({Str, Map.size()});
  return Iter->second;
}

uint64_t DataAccessProfData::addSymbolName(StringRef SymbolName) {
  return addStringRef(SymbolName, SymbolNames);
}
uint64_t DataAccessProfData::addFileName(StringRef FileName) {
  return addStringRef(FileName, FileNames);
}

DataAccessProfRecord &DataAccessProfData::addRecord(uint64_t SymbolNameIndex,
                                                    uint64_t StringContentHash,
                                                    uint64_t AccessCount) {
  Records.push_back({SymbolNameIndex, StringContentHash, AccessCount});
  return Records.back();
}

SmallVector<StringRef> DataAccessProfData::getSymbolNames() const {
  return llvm::to_vector(llvm::map_range(
      SymbolNames, [](const auto &Pair) { return Pair.first; }));
}

SmallVector<StringRef> DataAccessProfData::getFileNames() const {
  return llvm::to_vector(
      llvm::map_range(FileNames, [](const auto &Pair) { return Pair.first; }));
}
} // namespace llvm
