import type { Warning } from "../types";

interface Props {
  warnings: Warning[];
  suggestions: string[];
}

export function WarningsPanel({ warnings, suggestions }: Props) {
  if (warnings.length === 0 && suggestions.length === 0) {
    return <p className="text-muted-foreground text-sm">No warnings or suggestions.</p>;
  }

  return (
    <div className="space-y-4">
      {warnings.length > 0 && (
        <section>
          <h3 className="text-xs font-semibold uppercase tracking-wider text-yellow-400 mb-2">
            Warnings ({warnings.length})
          </h3>
          <div className="space-y-2">
            {warnings.map((w, i) => (
              <div key={i} className="flex items-start gap-3 rounded-lg border border-yellow-500/30 bg-yellow-500/10 px-4 py-3 text-yellow-300">
                <svg className="mt-0.5 h-4 w-4 flex-shrink-0" fill="none" viewBox="0 0 24 24" stroke="currentColor" strokeWidth={2}>
                  <path strokeLinecap="round" strokeLinejoin="round" d="M12 9v2m0 4h.01m-6.938 4h13.856c1.54 0 2.502-1.667 1.732-3L13.732 4c-.77-1.333-2.694-1.333-3.464 0L3.34 16c-.77 1.333.192 3 1.732 3z" />
                </svg>
                <div>
                  <p className="text-sm font-medium">{w.message}</p>
                  <p className="text-xs opacity-70 mt-0.5">Line {w.line} &bull; {w.category}</p>
                </div>
              </div>
            ))}
          </div>
        </section>
      )}

      {suggestions.length > 0 && (
        <section>
          <h3 className="text-xs font-semibold uppercase tracking-wider text-green-400 mb-2">
            Suggestions ({suggestions.length})
          </h3>
          <div className="space-y-2">
            {suggestions.map((s, i) => (
              <div key={i} className="flex items-start gap-3 rounded-lg border border-green-500/30 bg-green-500/10 px-4 py-3 text-green-300">
                <svg className="mt-0.5 h-4 w-4 flex-shrink-0" fill="none" viewBox="0 0 24 24" stroke="currentColor" strokeWidth={2}>
                  <path strokeLinecap="round" strokeLinejoin="round" d="M9.663 17h4.673M12 3v1m6.364 1.636l-.707.707M21 12h-1M4 12H3m3.343-5.657l-.707-.707m2.828 9.9a5 5 0 117.072 0l-.548.547A3.374 3.374 0 0014 18.469V19a2 2 0 11-4 0v-.531c0-.895-.356-1.754-.988-2.386l-.548-.547z" />
                </svg>
                <p className="text-sm">{s}</p>
              </div>
            ))}
          </div>
        </section>
      )}
    </div>
  );
}
