import type { SymbolEntry } from "../types";

interface Props {
  symbolTable: SymbolEntry[];
}

export function SymbolTablePanel({ symbolTable }: Props) {
  if (symbolTable.length === 0) {
    return <p className="text-muted-foreground text-sm">No symbols found.</p>;
  }

  return (
    <div className="overflow-auto">
      <table className="w-full text-sm">
        <thead>
          <tr className="border-b border-border">
            <th className="py-2 px-3 text-left font-semibold text-muted-foreground text-xs uppercase tracking-wider">Name</th>
            <th className="py-2 px-3 text-left font-semibold text-muted-foreground text-xs uppercase tracking-wider">Type</th>
            <th className="py-2 px-3 text-left font-semibold text-muted-foreground text-xs uppercase tracking-wider">Scope</th>
            <th className="py-2 px-3 text-left font-semibold text-muted-foreground text-xs uppercase tracking-wider">Line</th>
            <th className="py-2 px-3 text-left font-semibold text-muted-foreground text-xs uppercase tracking-wider">Kind</th>
            <th className="py-2 px-3 text-left font-semibold text-muted-foreground text-xs uppercase tracking-wider">Used</th>
          </tr>
        </thead>
        <tbody>
          {symbolTable.map((sym, i) => (
            <tr key={i} className="border-b border-border/50 hover:bg-muted/30 transition-colors">
              <td className="py-2 px-3 font-mono text-blue-300">{sym.name}</td>
              <td className="py-2 px-3 font-mono text-purple-400">{sym.type}</td>
              <td className="py-2 px-3 text-muted-foreground">{sym.scope}</td>
              <td className="py-2 px-3 text-muted-foreground">{sym.declarationLine}</td>
              <td className="py-2 px-3">
                <span className={`inline-flex items-center rounded-full px-2 py-0.5 text-xs font-medium ${
                  sym.isFunction
                    ? "bg-blue-500/20 text-blue-300"
                    : "bg-slate-500/20 text-slate-300"
                }`}>
                  {sym.isFunction ? "function" : "variable"}
                </span>
              </td>
              <td className="py-2 px-3">
                <span className={`inline-flex items-center rounded-full px-2 py-0.5 text-xs font-medium ${
                  sym.isUsed
                    ? "bg-green-500/20 text-green-300"
                    : "bg-yellow-500/20 text-yellow-300"
                }`}>
                  {sym.isUsed ? "yes" : "no"}
                </span>
              </td>
            </tr>
          ))}
        </tbody>
      </table>
    </div>
  );
}
