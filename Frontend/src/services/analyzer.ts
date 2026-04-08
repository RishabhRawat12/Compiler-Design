import type { AnalysisResult } from "../types";

export async function analyzeCode(code: string): Promise<AnalysisResult> {
  const response = await fetch("http://localhost:3001/api/analyze", {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify({ code }),
  });

  if (!response.ok) {
    const err = await response.json().catch(() => ({ error: "Unknown error" }));
    throw new Error(err.error || "Analysis failed");
  }

  return response.json();
}