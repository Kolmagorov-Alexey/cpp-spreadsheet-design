#pragma once

#include "common.h"
#include "formula.h"
#include <functional>
#include <unordered_set>
#include <optional>

class Sheet;

class Cell : public CellInterface {
public:
    Cell(Sheet& sheet);
    ~Cell();
    void Set(std::string text);
    void Clear();
    Value GetValue() const override;
    std::string GetText() const override;
    std::vector<Position> GetReferencedCells() const override;

private:
    class Impl;
    class EmptyImpl;
    class TextImpl;
    class FormulaImpl;
    bool WouldIntroduceCircularDependency(const Impl& impl) const;
    void InvalidateCacheRecursive(bool);
    
private:
    std::unique_ptr<Impl> impl_;
    Sheet& sheet_;
    std::unordered_set<Cell*> l_nodes_;
    std::unordered_set<Cell*> r_nodes_;
};

class Cell::Impl {
public:
  virtual Cell::Value GetValue() const = 0;
  virtual std::string GetText() const  = 0;
  virtual std::vector<Position> GetReferencedCells() const;
  virtual bool IsCacheValid() const =0
  virtual void InvalidateCache() = 0
  virtual ~Impl() = default;
};
class Cell::EmptyImpl : public Cell::Impl {
public:
    Cell::Value GetValue() const override;
    std::string GetText() const override;
};
class Cell::TextImpl : public Cell::Impl {
public:
    TextImpl(std::string text);
    Cell::Value GetValue()  const override;
    std::string GetText()  const override;
private:
    std::string text_;
};
class Cell::FormulaImpl : public Cell::Impl {
public:
    FormulaImpl(std::string f_text, const SheetInterface& sheet);
    Cell::Value GetValue() const override;
    std::string GetText() const override;
	bool IsCacheValid() const override ;
    void InvalidateCache() override;
    std::vector<Position> GetReferencedCells() const
private:
   std::unique_ptr<FormulaInterface> formula_ptr_;
   const SheetInterface& sheet_;
   mutable std::optional<FormulaInterface::Value> cache_;
};



