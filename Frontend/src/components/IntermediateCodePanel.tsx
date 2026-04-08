interface Props {
  code: string[];
}

export function IntermediateCodePanel({ code }: Props) {
  if (code.length === 0) {
    return <p className="text-muted-foreground text-sm">No intermediate code generated. Fix errors first.</p>;
  }

  return (
    <div className="overflow-auto">
      <div className="rounded-lg border border-border bg-card font-mono text-sm">
        {code.map((line, i) => {
          const isLabel = line.trim().endsWith(":");
          const isFunc = line.startsWith("begin ") || line.startsWith("end ");
          const isGoto = line.startsWith("goto") || line.includes(" goto ");
          const isReturn = line.startsWith("return");
          return (
            <div
              key={i}
              className={`flex items-start gap-4 px-4 py-1.5 border-b border-border/30 last:border-0 ${
                isLabel ? "bg-primary/10 text-primary font-semibold" :
                isFunc ? "bg-purple-500/10 text-purple-300 font-semibold" :
                isGoto ? "text-yellow-300" :
                isReturn ? "text-green-300" :
                "text-slate-300"
              }`}
            >
              <span className="text-muted-foreground text-xs select-none w-8 flex-shrink-0 mt-0.5 text-right">
                {i + 1}
              </span>
              <span className="flex-1">{line}</span>
            </div>
          );
        })}
      </div>
      <p className="mt-2 text-xs text-muted-foreground">{code.length} TAC instructions</p>
    </div>
  );
}
