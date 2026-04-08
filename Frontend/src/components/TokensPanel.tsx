import type { Token } from "../types";

const tokenColors: Record<string, string> = {
  KW_INT: "text-purple-400",
  KW_FLOAT: "text-purple-400",
  KW_CHAR: "text-purple-400",
  KW_VOID: "text-purple-400",
  KW_IF: "text-purple-400",
  KW_ELSE: "text-purple-400",
  KW_WHILE: "text-purple-400",
  KW_FOR: "text-purple-400",
  KW_RETURN: "text-purple-400",
  KW_BREAK: "text-purple-400",
  KW_CONTINUE: "text-purple-400",
  KW_DO: "text-purple-400",
  KW_STRUCT: "text-purple-400",
  KW_TYPEDEF: "text-purple-400",
  IDENTIFIER: "text-blue-300",
  INT_LITERAL: "text-amber-400",
  FLOAT_LITERAL: "text-amber-400",
  CHAR_LITERAL: "text-green-400",
  STRING_LITERAL: "text-green-400",
  OP_ASSIGN: "text-orange-400",
  OP_EQ: "text-orange-400",
  OP_NEQ: "text-orange-400",
  OP_LT: "text-orange-400",
  OP_LE: "text-orange-400",
  OP_GT: "text-orange-400",
  OP_GE: "text-orange-400",
  OP_AND: "text-orange-400",
  OP_OR: "text-orange-400",
  OP_NOT: "text-orange-400",
  SEMICOLON: "text-slate-400",
  COMMA: "text-slate-400",
  LPAREN: "text-yellow-400",
  RPAREN: "text-yellow-400",
  LBRACE: "text-yellow-400",
  RBRACE: "text-yellow-400",
  LBRACKET: "text-yellow-400",
  RBRACKET: "text-yellow-400",
};

function getTokenColor(type: string): string {
  return tokenColors[type] || "text-slate-300";
}

interface Props {
  tokens: Token[];
}

export function TokensPanel({ tokens }: Props) {
  if (tokens.length === 0) {
    return <p className="text-muted-foreground text-sm">No tokens yet. Run analysis first.</p>;
  }

  return (
    <div className="overflow-auto">
      <div className="flex flex-wrap gap-1.5">
        {tokens.map((token, i) => (
          <span
            key={i}
            className={`inline-flex flex-col items-center rounded border border-border bg-card px-2 py-1 text-xs font-mono group relative cursor-default ${getTokenColor(token.type)}`}
            title={`Line ${token.line}, Col ${token.column}`}
          >
            <span className="font-semibold">{token.value || <em className="opacity-50">(empty)</em>}</span>
            <span className="text-muted-foreground text-[10px]">{token.type}</span>
          </span>
        ))}
      </div>
      <p className="mt-3 text-xs text-muted-foreground">{tokens.length} tokens</p>
    </div>
  );
}
