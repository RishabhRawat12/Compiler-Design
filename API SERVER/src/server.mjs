import express from "express";
import cors from "cors";
import { spawn } from "child_process";
import { existsSync } from "fs";
import { fileURLToPath } from "url";
import path from "path";

const app = express();
app.use(cors());
app.use(express.json());

const __dirname = path.dirname(fileURLToPath(import.meta.url));
const ANALYZER = path.resolve(__dirname, "../backend/analyzer");

app.post("/api/analyze", async (req, res) => {
  const { code } = req.body;
  if (!code) return res.status(400).json({ error: "Missing code" });

  if (!existsSync(ANALYZER)) {
    return res.status(500).json({ error: "Analyzer binary not found. Run: make -C backend" });
  }

  const child = spawn(ANALYZER, [], { timeout: 10000 });
  let out = "", err = "";
  child.stdout.on("data", d => out += d);
  child.stderr.on("data", d => err += d);
  child.on("close", code => {
    if (code !== 0) return res.status(500).json({ error: err });
    try { res.json(JSON.parse(out)); }
    catch { res.status(500).json({ error: "Bad output: " + out.slice(0, 200) }); }
  });
  child.stdin.write(code);
  child.stdin.end();
});

app.listen(3001, () => console.log("API running on http://localhost:3001"));