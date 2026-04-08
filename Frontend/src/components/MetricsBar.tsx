import type { Metrics } from "../types";

interface Props {
  metrics: Metrics;
}

function MetricCard({ label, value, color, tooltip }: { label: string; value: number; color: string; tooltip: string }) {
  return (
    <div className={`flex flex-col items-center rounded-lg border px-6 py-4 ${color}`} title={tooltip}>
      <span className="text-2xl font-bold tabular-nums">{value}</span>
      <span className="text-xs mt-1 opacity-80">{label}</span>
    </div>
  );
}

export function MetricsBar({ metrics }: Props) {
  const ccColor = metrics.cyclomaticComplexity <= 5
    ? "border-green-500/30 bg-green-500/10 text-green-300"
    : metrics.cyclomaticComplexity <= 10
    ? "border-yellow-500/30 bg-yellow-500/10 text-yellow-300"
    : "border-red-500/30 bg-red-500/10 text-red-300";

  const ndColor = metrics.maxNestingDepth <= 3
    ? "border-green-500/30 bg-green-500/10 text-green-300"
    : metrics.maxNestingDepth <= 5
    ? "border-yellow-500/30 bg-yellow-500/10 text-yellow-300"
    : "border-red-500/30 bg-red-500/10 text-red-300";

  return (
    <div className="flex gap-3 flex-wrap">
      <MetricCard
        label="Cyclomatic Complexity"
        value={metrics.cyclomaticComplexity}
        color={ccColor}
        tooltip="Measures the number of linearly independent paths. Low = good."
      />
      <MetricCard
        label="Max Nesting Depth"
        value={metrics.maxNestingDepth}
        color={ndColor}
        tooltip="Maximum nesting level of control structures."
      />
      <MetricCard
        label="Functions"
        value={metrics.functionCount}
        color="border-blue-500/30 bg-blue-500/10 text-blue-300"
        tooltip="Total number of function definitions."
      />
    </div>
  );
}
