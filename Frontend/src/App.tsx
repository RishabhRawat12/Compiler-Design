import { useState, useCallback } from "react";
import { QueryClient, QueryClientProvider } from "@tanstack/react-query";
import MonacoEditor from "@monaco-editor/react";
import { analyzeCode } from "./services/analyzer";
import type { AnalysisResult } from "./types";
import { TokensPanel } from "./components/TokensPanel";
import { ErrorsPanel } from "./components/ErrorsPanel";
import { WarningsPanel } from "./components/WarningsPanel";
import { SymbolTablePanel } from "./components/SymbolTablePanel";
import { ParseTreePanel } from "./components/ParseTreePanel";
import { IntermediateCodePanel } from "./components/IntermediateCodePanel";
import { MetricsBar } from "./components/MetricsBar";

const queryClient = new QueryClient();

const SAMPLE_CODES: Record<string, string> = {
  "Basic": `int add(int a, int b) {
    return a + b;
}

int main() {
    int x = 10;
    int y = 20;
    int result = add(x, y);
    return result;
}`,
  "With Errors": `int main() {
    int x = 5;
    y = x + 1;
    return undeclared;
}`,
  "Complex": `int factorial(int n) {
    if (n <= 1) {
        return 1;
    }
    return n * factorial(n - 1);
}

int fibonacci(int n) {
    if (n <= 1) return n;
    int a = 0;
    int b = 1;
    int i = 2;
    while (i <= n) {
        int temp = a + b;
        a = b;
        b = temp;
        i = i + 1;
    }
    return b;
}

int main() {
    int x = factorial(5);
    int y = fibonacci(10);
    return x + y;
}`,
  "Dead Code": `int main() {
    int x = 5;
    int unused = 99;
    if (x > 10) {
        return 1;
    }
    return 0;
}`,
};

const TABS = [
  { id: "tokens", label: "Tokens" },
  { id: "errors", label: "Errors" },
  { id: "warnings", label: "Warnings" },
  { id: "symbols", label: "Symbol Table" },
  { id: "parsetree", label: "Parse Tree" },
  { id: "tac", label: "Intermediate Code" },
];

function getErrorBadge(result: AnalysisResult | null): number {
  if (!result) return 0;
  return result.lexicalErrors.length + result.syntaxErrors.length + result.semanticErrors.length;
}

function getWarningBadge(result: AnalysisResult | null): number {
  if (!result) return 0;
  return result.warnings.length + result.suggestions.length;
}

function AppContent() {
  const [code, setCode] = useState(SAMPLE_CODES["Basic"]);
  const [activeTab, setActiveTab] = useState("tokens");
  const [result, setResult] = useState<AnalysisResult | null>(null);
  const [isLoading, setIsLoading] = useState(false);
  const [error, setError] = useState<string | null>(null);

  const handleAnalyze = useCallback(async () => {
    if (!code.trim()) return;
    setIsLoading(true);
    setError(null);
    try {
      const data = await analyzeCode(code);
      setResult(data);
    } catch (e) {
      setError(e instanceof Error ? e.message : String(e));
    } finally {
      setIsLoading(false);
    }
  }, [code]);

  const totalErrors = getErrorBadge(result);
  const totalWarnings = getWarningBadge(result);

  function getBadge(tabId: string): number | null {
    if (!result) return null;
    if (tabId === "errors") return totalErrors || null;
    if (tabId === "warnings") return totalWarnings || null;
    if (tabId === "tokens") return result.tokens.length || null;
    if (tabId === "symbols") return result.symbolTable.length || null;
    if (tabId === "tac") return result.intermediateCode.length || null;
    return null;
  }

  return (
    <div className="flex flex-col min-h-screen bg-background text-foreground">
      {/* Header */}
      <header className="border-b border-border bg-card px-6 py-4">
        <div className="flex items-center justify-between max-w-screen-2xl mx-auto">
          <div className="flex items-center gap-3">
            <div className="rounded-lg bg-primary/20 p-2">
              <svg className="h-5 w-5 text-primary" fill="none" viewBox="0 0 24 24" stroke="currentColor" strokeWidth={2}>
                <path strokeLinecap="round" strokeLinejoin="round" d="M10 20l4-16m4 4l4 4-4 4M6 16l-4-4 4-4" />
              </svg>
            </div>
            <div>
              <h1 className="text-lg font-bold tracking-tight">Static Code Analyzer</h1>
              <p className="text-xs text-muted-foreground">C language compiler phases — powered by C++ backend</p>
            </div>
          </div>
          <div className="flex items-center gap-2">
            <span className="text-xs text-muted-foreground mr-2 hidden sm:block">Sample:</span>
            {Object.keys(SAMPLE_CODES).map(name => (
              <button
                key={name}
                onClick={() => { setCode(SAMPLE_CODES[name]); setResult(null); }}
                className="text-xs px-3 py-1.5 rounded border border-border bg-secondary hover:bg-accent hover:text-accent-foreground transition-colors"
              >
                {name}
              </button>
            ))}
          </div>
        </div>
      </header>

      <main className="flex-1 flex flex-col lg:flex-row gap-0 max-w-screen-2xl mx-auto w-full">
        {/* Editor panel */}
        <div className="lg:w-1/2 xl:w-2/5 flex flex-col border-r border-border">
          <div className="flex items-center justify-between px-4 py-3 border-b border-border bg-card/50">
            <span className="text-sm font-medium text-muted-foreground">C Source Code</span>
            <button
              onClick={handleAnalyze}
              disabled={isLoading}
              className="flex items-center gap-2 rounded-lg bg-primary px-5 py-2 text-sm font-semibold text-primary-foreground hover:brightness-110 disabled:opacity-50 disabled:cursor-not-allowed transition-all"
            >
              {isLoading ? (
                <>
                  <svg className="h-4 w-4 animate-spin" fill="none" viewBox="0 0 24 24">
                    <circle className="opacity-25" cx="12" cy="12" r="10" stroke="currentColor" strokeWidth="4" />
                    <path className="opacity-75" fill="currentColor" d="M4 12a8 8 0 018-8V0C5.373 0 0 5.373 0 12h4z" />
                  </svg>
                  Analyzing...
                </>
              ) : (
                <>
                  <svg className="h-4 w-4" fill="none" viewBox="0 0 24 24" stroke="currentColor" strokeWidth={2}>
                    <path strokeLinecap="round" strokeLinejoin="round" d="M14.752 11.168l-3.197-2.132A1 1 0 0010 9.87v4.263a1 1 0 001.555.832l3.197-2.132a1 1 0 000-1.664z" />
                    <path strokeLinecap="round" strokeLinejoin="round" d="M21 12a9 9 0 11-18 0 9 9 0 0118 0z" />
                  </svg>
                  Analyze Code
                </>
              )}
            </button>
          </div>

          <div className="flex-1" style={{ minHeight: 400 }}>
            <MonacoEditor
              height="100%"
              defaultLanguage="c"
              theme="vs-dark"
              value={code}
              onChange={(v) => setCode(v ?? "")}
              options={{
                minimap: { enabled: false },
                fontSize: 14,
                fontFamily: "'JetBrains Mono', 'Fira Code', Menlo, monospace",
                lineNumbers: "on",
                scrollBeyondLastLine: false,
                wordWrap: "on",
                padding: { top: 12, bottom: 12 },
                renderLineHighlight: "gutter",
                overviewRulerLanes: 0,
              }}
            />
          </div>

          {error && (
            <div className="px-4 py-3 border-t border-red-500/30 bg-red-500/10 text-red-300 text-sm">
              <strong>Error:</strong> {error}
            </div>
          )}

          {result && (
            <div className="border-t border-border px-4 py-3">
              <MetricsBar metrics={result.metrics} />
            </div>
          )}
        </div>

        {/* Results panel */}
        <div className="lg:flex-1 flex flex-col">
          {/* Tabs */}
          <div className="flex border-b border-border bg-card/50 overflow-x-auto">
            {TABS.map(tab => {
              const badge = getBadge(tab.id);
              const isError = tab.id === "errors" && totalErrors > 0;
              const isWarning = tab.id === "warnings" && totalWarnings > 0;
              return (
                <button
                  key={tab.id}
                  onClick={() => setActiveTab(tab.id)}
                  className={`flex items-center gap-1.5 px-4 py-3 text-sm font-medium whitespace-nowrap border-b-2 transition-colors ${
                    activeTab === tab.id
                      ? "border-primary text-primary"
                      : "border-transparent text-muted-foreground hover:text-foreground hover:border-border"
                  }`}
                >
                  {tab.label}
                  {badge !== null && badge > 0 && (
                    <span className={`inline-flex items-center justify-center rounded-full px-1.5 py-0.5 text-xs font-bold min-w-[18px] ${
                      isError ? "bg-red-500/20 text-red-300" :
                      isWarning ? "bg-yellow-500/20 text-yellow-300" :
                      "bg-primary/20 text-primary"
                    }`}>
                      {badge}
                    </span>
                  )}
                </button>
              );
            })}
          </div>

          {/* Tab content */}
          <div className="flex-1 overflow-auto p-4">
            {!result && !isLoading && (
              <div className="flex flex-col items-center justify-center h-full text-center">
                <div className="rounded-full bg-primary/10 p-6 mb-4">
                  <svg className="h-10 w-10 text-primary/60" fill="none" viewBox="0 0 24 24" stroke="currentColor" strokeWidth={1.5}>
                    <path strokeLinecap="round" strokeLinejoin="round" d="M9 5H7a2 2 0 00-2 2v12a2 2 0 002 2h10a2 2 0 002-2V7a2 2 0 00-2-2h-2M9 5a2 2 0 002 2h2a2 2 0 002-2M9 5a2 2 0 012-2h2a2 2 0 012 2m-3 7h3m-3 4h3m-6-4h.01M9 16h.01" />
                  </svg>
                </div>
                <h2 className="text-lg font-semibold text-foreground/70 mb-1">Ready to analyze</h2>
                <p className="text-sm text-muted-foreground max-w-xs">
                  Write or paste C code in the editor, then click "Analyze Code" to run all compiler phases.
                </p>
              </div>
            )}

            {isLoading && (
              <div className="flex flex-col items-center justify-center h-full text-center">
                <svg className="h-10 w-10 text-primary animate-spin mb-4" fill="none" viewBox="0 0 24 24">
                  <circle className="opacity-25" cx="12" cy="12" r="10" stroke="currentColor" strokeWidth="4" />
                  <path className="opacity-75" fill="currentColor" d="M4 12a8 8 0 018-8V0C5.373 0 0 5.373 0 12h4z" />
                </svg>
                <p className="text-sm text-muted-foreground">Running compiler phases...</p>
              </div>
            )}

            {result && !isLoading && (
              <>
                {activeTab === "tokens" && <TokensPanel tokens={result.tokens} />}
                {activeTab === "errors" && (
                  <ErrorsPanel
                    lexicalErrors={result.lexicalErrors}
                    syntaxErrors={result.syntaxErrors}
                    semanticErrors={result.semanticErrors}
                  />
                )}
                {activeTab === "warnings" && (
                  <WarningsPanel warnings={result.warnings} suggestions={result.suggestions} />
                )}
                {activeTab === "symbols" && <SymbolTablePanel symbolTable={result.symbolTable} />}
                {activeTab === "parsetree" && <ParseTreePanel root={result.parseTree} />}
                {activeTab === "tac" && <IntermediateCodePanel code={result.intermediateCode} />}
              </>
            )}
          </div>
        </div>
      </main>
    </div>
  );
}

export default function App() {
  return (
    <QueryClientProvider client={queryClient}>
      <AppContent />
    </QueryClientProvider>
  );
}
