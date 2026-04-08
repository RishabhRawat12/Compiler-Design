export interface Token {
  type: string;
  value: string;
  line: number;
  column: number;
}

export interface LexicalError {
  message: string;
  line: number;
  column: number;
}

export interface SyntaxError {
  message: string;
  line: number;
  column: number;
}

export interface SemanticError {
  message: string;
  line: number;
  column: number;
  severity: string;
}

export interface Warning {
  message: string;
  line: number;
  column: number;
  category: string;
}

export interface SymbolEntry {
  name: string;
  type: string;
  scope: string;
  declarationLine: number;
  isUsed: boolean;
  isFunction: boolean;
}

export interface ASTNode {
  type: string;
  value: string;
  line: number;
  column: number;
  children: ASTNode[];
}

export interface Metrics {
  cyclomaticComplexity: number;
  maxNestingDepth: number;
  functionCount: number;
}

export interface AnalysisResult {
  tokens: Token[];
  lexicalErrors: LexicalError[];
  syntaxErrors: SyntaxError[];
  semanticErrors: SemanticError[];
  warnings: Warning[];
  symbolTable: SymbolEntry[];
  parseTree: ASTNode | null;
  intermediateCode: string[];
  suggestions: string[];
  metrics: Metrics;
}
