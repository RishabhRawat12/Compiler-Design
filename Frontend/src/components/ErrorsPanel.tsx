import type { LexicalError, SyntaxError as SErr, SemanticError } from "../types";

interface Props {
  lexicalErrors: LexicalError[];
  syntaxErrors: SErr[];
  semanticErrors: SemanticError[];
}

function ErrorItem({ message, line, column, color }: { message: string; line: number; column: number; color: string }) {
  return (
    <div className={`flex items-start gap-3 rounded-lg border px-4 py-3 ${color}`}>
      <svg className="mt-0.5 h-4 w-4 flex-shrink-0" fill="none" viewBox="0 0 24 24" stroke="currentColor" strokeWidth={2}>
        <circle cx="12" cy="12" r="10" />
        <line x1="12" y1="8" x2="12" y2="12" />
        <line x1="12" y1="16" x2="12.01" y2="16" />
      </svg>
      <div className="flex-1 min-w-0">
        <p className="text-sm font-medium">{message}</p>
        <p className="text-xs opacity-70 mt-0.5">Line {line}, Column {column}</p>
      </div>
    </div>
  );
}

export function ErrorsPanel({ lexicalErrors, syntaxErrors, semanticErrors }: Props) {
  const total = lexicalErrors.length + syntaxErrors.length + semanticErrors.length;

  if (total === 0) {
    return (
      <div className="flex items-center gap-3 rounded-lg border border-green-500/30 bg-green-500/10 px-4 py-3 text-green-400">
        <svg className="h-5 w-5 flex-shrink-0" fill="none" viewBox="0 0 24 24" stroke="currentColor" strokeWidth={2}>
          <path strokeLinecap="round" strokeLinejoin="round" d="M9 12l2 2 4-4m6 2a9 9 0 11-18 0 9 9 0 0118 0z" />
        </svg>
        <span className="text-sm font-medium">No errors detected</span>
      </div>
    );
  }

  return (
    <div className="space-y-4">
      {lexicalErrors.length > 0 && (
        <section>
          <h3 className="text-xs font-semibold uppercase tracking-wider text-red-400 mb-2">
            Lexical Errors ({lexicalErrors.length})
          </h3>
          <div className="space-y-2">
            {lexicalErrors.map((e, i) => (
              <ErrorItem key={i} {...e} color="border-red-500/30 bg-red-500/10 text-red-300" />
            ))}
          </div>
        </section>
      )}

      {syntaxErrors.length > 0 && (
        <section>
          <h3 className="text-xs font-semibold uppercase tracking-wider text-red-400 mb-2">
            Syntax Errors ({syntaxErrors.length})
          </h3>
          <div className="space-y-2">
            {syntaxErrors.map((e, i) => (
              <ErrorItem key={i} {...e} color="border-red-500/30 bg-red-500/10 text-red-300" />
            ))}
          </div>
        </section>
      )}

      {semanticErrors.length > 0 && (
        <section>
          <h3 className="text-xs font-semibold uppercase tracking-wider text-orange-400 mb-2">
            Semantic Errors ({semanticErrors.length})
          </h3>
          <div className="space-y-2">
            {semanticErrors.map((e, i) => (
              <ErrorItem key={i} message={e.message} line={e.line} column={e.column} color="border-orange-500/30 bg-orange-500/10 text-orange-300" />
            ))}
          </div>
        </section>
      )}
    </div>
  );
}
