import { Router } from "express";
import { spawn } from "child_process";
import { existsSync } from "fs";
import path from "path";
import { fileURLToPath } from "url";

const router = Router();

const __srcDir = path.dirname(fileURLToPath(import.meta.url));
const CANDIDATE_PATHS = [
  path.resolve(__srcDir, "../../backend/analyzer"),
  path.resolve(__srcDir, "../../../backend/analyzer"),
  "/home/runner/workspace/artifacts/api-server/backend/analyzer",
];

function getAnalyzerPath(): string {
  for (const p of CANDIDATE_PATHS) {
    if (existsSync(p)) return p;
  }
  return CANDIDATE_PATHS[CANDIDATE_PATHS.length - 1];
}

const ANALYZER_PATH = getAnalyzerPath();

router.post("/analyze", async (req, res) => {
  const { code } = req.body;

  if (!code || typeof code !== "string") {
    res.status(400).json({ error: "Missing or invalid 'code' field in request body" });
    return;
  }

  try {
    const result = await runAnalyzer(code);
    res.json(result);
  } catch (err: unknown) {
    req.log.error({ err }, "Analysis failed");
    res.status(500).json({ error: "Analysis failed", details: String(err) });
  }
});

function runAnalyzer(code: string): Promise<unknown> {
  return new Promise((resolve, reject) => {
    const child = spawn(ANALYZER_PATH, [], { timeout: 10000 });

    let stdout = "";
    let stderr = "";

    child.stdout.on("data", (data: Buffer) => {
      stdout += data.toString();
    });

    child.stderr.on("data", (data: Buffer) => {
      stderr += data.toString();
    });

    child.on("close", (exitCode) => {
      if (exitCode !== 0) {
        reject(new Error(`Analyzer exited with code ${exitCode}: ${stderr}`));
        return;
      }
      try {
        resolve(JSON.parse(stdout));
      } catch {
        reject(new Error("Failed to parse analyzer output: " + stdout.slice(0, 200)));
      }
    });

    child.on("error", (err) => {
      reject(err);
    });

    child.stdin.write(code);
    child.stdin.end();
  });
}

export default router;
