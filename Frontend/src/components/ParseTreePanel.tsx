import { useEffect, useRef } from "react";
import * as d3 from "d3";
import type { ASTNode } from "../types";

interface Props {
  root: ASTNode | null;
}

const NODE_COLORS: Record<string, string> = {
  Program: "#60a5fa",
  FunctionDef: "#a78bfa",
  Block: "#34d399",
  Declaration: "#fb923c",
  If: "#f472b6",
  While: "#f472b6",
  For: "#f472b6",
  DoWhile: "#f472b6",
  Return: "#facc15",
  BinaryOp: "#64748b",
  Assign: "#64748b",
  Identifier: "#38bdf8",
  IntLiteral: "#fb923c",
  FloatLiteral: "#fb923c",
  CharLiteral: "#4ade80",
  StringLiteral: "#4ade80",
  FuncCall: "#818cf8",
};

function getNodeColor(type: string): string {
  return NODE_COLORS[type] || "#94a3b8";
}

interface D3ASTNode extends d3.HierarchyNode<ASTNode> {}

export function ParseTreePanel({ root }: Props) {
  const svgRef = useRef<SVGSVGElement>(null);
  const containerRef = useRef<HTMLDivElement>(null);

  useEffect(() => {
    if (!root || !svgRef.current || !containerRef.current) return;

    const container = containerRef.current;
    const width = container.clientWidth || 800;
    const height = 600;

    const svg = d3.select(svgRef.current)
      .attr("width", width)
      .attr("height", height);

    svg.selectAll("*").remove();

    const g = svg.append("g");

    const zoom = d3.zoom<SVGSVGElement, unknown>()
      .scaleExtent([0.1, 3])
      .on("zoom", (event) => {
        g.attr("transform", event.transform);
      });

    svg.call(zoom);

    const hierarchy = d3.hierarchy<ASTNode>(root, d => d.children);
    const treeLayout = d3.tree<ASTNode>().nodeSize([160, 80]);
    const treeData = treeLayout(hierarchy);

    // Draw links
    g.append("g")
      .attr("fill", "none")
      .attr("stroke", "#475569")
      .attr("stroke-width", 1.5)
      .selectAll("path")
      .data(treeData.links())
      .join("path")
      .attr("d", d3.linkVertical<d3.HierarchyLink<ASTNode>, d3.HierarchyPointNode<ASTNode>>()
        .x(d => d.x)
        .y(d => d.y)
      );

    // Draw nodes
    const node = g.append("g")
      .selectAll("g")
      .data(treeData.descendants())
      .join("g")
      .attr("transform", d => `translate(${d.x},${d.y})`);

    node.append("rect")
      .attr("x", -50)
      .attr("y", -18)
      .attr("width", 100)
      .attr("height", 36)
      .attr("rx", 6)
      .attr("fill", d => getNodeColor(d.data.type))
      .attr("fill-opacity", 0.2)
      .attr("stroke", d => getNodeColor(d.data.type))
      .attr("stroke-width", 1.5);

    node.append("text")
      .attr("text-anchor", "middle")
      .attr("dominant-baseline", "middle")
      .attr("dy", -5)
      .attr("fill", d => getNodeColor(d.data.type))
      .attr("font-size", "10px")
      .attr("font-weight", "600")
      .attr("font-family", "monospace")
      .text(d => d.data.type);

    node.append("text")
      .attr("text-anchor", "middle")
      .attr("dominant-baseline", "middle")
      .attr("dy", 8)
      .attr("fill", "#94a3b8")
      .attr("font-size", "9px")
      .attr("font-family", "monospace")
      .text(d => d.data.value ? d.data.value.slice(0, 12) : "");

    // Center the view
    const bounds = (g.node() as SVGGElement)?.getBBox();
    if (bounds) {
      const tx = width / 2 - bounds.x - bounds.width / 2;
      const ty = 40;
      svg.call(zoom.transform, d3.zoomIdentity.translate(tx, ty).scale(0.8));
    }
  }, [root]);

  if (!root) {
    return <p className="text-muted-foreground text-sm">No parse tree. Fix syntax errors first.</p>;
  }

  return (
    <div className="w-full" ref={containerRef}>
      <p className="text-xs text-muted-foreground mb-2">Scroll to zoom, drag to pan.</p>
      <div className="rounded-lg border border-border overflow-hidden bg-card">
        <svg ref={svgRef} className="w-full" style={{ minHeight: 500 }} />
      </div>
    </div>
  );
}
