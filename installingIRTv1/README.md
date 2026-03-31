# drich-dev with IRTv1 — Installation & Validation Guide

This repository provides a **reproducible workflow** to install, configure, and validate `drich-dev` using **IRTv1-compatible commits**.

---

## 📦 Prerequisites

- Linux / macOS / WSL environment  
- `git`, `curl`, `wget` installed  
- Basic terminal familiarity  

---

## 📥 1. Download Setup Script

```bash
wget https://raw.githubusercontent.com/kumardeepraman/learning-drich-dev/main/installingIRTv1/setup_steps.sh
chmod +x setup_steps.sh
```

---

## 🧩 2. Install EPIC Environment (Module A)

```bash
./setup_steps.sh --A
```

This installs the **eic-shell environment**.

### ✔ Verify:

```bash
./eic-shell --version
```

Expected output:

```
26.03.1-stable
```

---

## 🌐 3. Enter eic-shell

```bash
./eic-shell
```

---

## 📁 4. Clone drich-dev (Module B)

```bash
./setup_steps.sh --B
cd drich-dev
```

---

## 🔗 5. Move Setup Script

```bash
mv ../setup_steps.sh .
```

---

## 📚 6. Clone Dependencies (Module C)

```bash
./setup_steps.sh --C
```

This step:
- Clones all required repositories  
- Checks out **fixed commits (IRTv1 compatible)**  
- Applies necessary file replacements  

---

## 🌱 7. Load Environment

```bash
source environ.sh
```

---

## ⚙️ 8. Build Core Components (Module D)

```bash
./setup_steps.sh --D
```

Builds:
- `epic`  
- `irt`  
- `EDM4eic`  
- `EICrecon`  
- `detector_benchmarks`  

---

## 🔄 9. Reload Environment

```bash
source environ.sh
```

---

## 🏁 10. Final Setup (Module E)

```bash
./setup_steps.sh --E
```

This step:
- Links benchmark script  
- Applies runtime fixes  

---

## 🧪 11. Run Simulation

```bash
./simulate.py -t1 -n10
```

---

## 🔄 12. Run Reconstruction

```bash
./recon.rb
```

---

## 📊 13. Run Benchmark

```bash
./benchmark2.rb -b
```

---

## ✅ Expected Output

- Simulation runs successfully  
- Reconstruction completes without errors  
- Benchmark produces output plots/results  

---

## ⚡ Quick Setup (Advanced Users)

```bash
wget https://raw.githubusercontent.com/kumardeepraman/learning-drich-dev/main/installingIRTv1/setup_steps.sh && \
chmod +x setup_steps.sh && \
./setup_steps.sh --A
```

Then inside `eic-shell`:

```bash
./setup_steps.sh --B && cd drich-dev && mv ../setup_steps.sh . && \
./setup_steps.sh --C && source environ.sh && \
./setup_steps.sh --D && source environ.sh && \
./setup_steps.sh --E
```

---

## 🛠️ Notes

- Always run:
  ```bash
  source environ.sh
  ```
  after build steps  




---

## 📌 Author
**Ramandeep Kumar**  
