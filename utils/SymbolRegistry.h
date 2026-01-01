#pragma once

#include <unordered_map>
#ifndef _SYMBOL_REGISTRY_H_
#define _SYMBOL_REGISTRY_H_

#include <string>
#include <vector>

class SymbolRegistry
{

  public:
    static SymbolRegistry &instance()
    {
        static SymbolRegistry instance;
        return instance;
    }

    int registerSymbol(const std::string &symbol)
    {
        if (m_symbolToId.find(symbol) == m_symbolToId.end())
        {
            int newId = m_idToSymbol.size() + 1;
            m_idToSymbol.push_back(symbol);
            m_symbolToId[symbol] = newId;
            return newId;
        }
        return m_symbolToId[symbol];
    }

    int getId(const std::string &symbol) const
    {
        auto it = m_symbolToId.find(symbol);
        if (it != m_symbolToId.end())
            return it->second;
        throw std::runtime_error("Symbol not found: " + symbol);
    }

    std::string getSymbol(int id) const
    {
        if (id > 0 && id <= m_idToSymbol.size())
        {
            return m_idToSymbol[id - 1];
        }
        return "UNKNOWN";
    }

  private:
    SymbolRegistry()
    {
    }
    std::vector<std::string> m_idToSymbol;
    std::unordered_map<std::string, int> m_symbolToId;
};

#endif // _SYMBOL_REGISTRY_H_
