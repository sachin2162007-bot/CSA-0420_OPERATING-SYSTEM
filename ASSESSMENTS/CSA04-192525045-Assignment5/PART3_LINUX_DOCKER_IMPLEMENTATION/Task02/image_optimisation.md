# B2 – Image Layers and Image-Size Reduction

**Status: PENDING for every measurement.** The image could not be built in the sandbox (Docker Hub unreachable, see `docker_commands.md`). This file contains the design of the optimisation, the commands, and an empty results table. **No size, time or package count is claimed.** `capture_evidence.sh` (steps 05 and 06) fills in the real values.

## 1. Inspecting layers (PENDING)
```bash
docker history result-portal
docker history --no-trunc --format '{{.Size}}\t{{.CreatedBy}}' result-portal
```
Expected structure (from the Dockerfile, not measured): the base image layers of `python:3.12-slim` (read-only, shared by every image using that base) and, on top, one layer per instruction that changes files: `pip install` (the large one: Flask, Gunicorn, psycopg2), `useradd` (tiny), `COPY . .` (a few KB). `WORKDIR`, `EXPOSE`, `USER` and `CMD` add metadata only (0 B). Because `COPY requirements.txt` and `pip install` come **before** `COPY . .`, editing `app.py` rebuilds only the last layer; the slow dependency layer comes from cache. This is the layer-caching reasoning given in the official Dockerfile comment.

## 2. Reduction steps already built into the submitted Dockerfile
| Step | Where | Effect |
|---|---|---|
| Slim base image (`python:3.12-slim`, official instruction) | `FROM` | Debian without most optional packages, compared with a full distribution image |
| `pip install --no-cache-dir` (official instruction) | `RUN pip install` | Does not keep the download cache in the layer |
| `.dockerignore` | `Task02/.dockerignore` | Keeps `.git`, docs, `database/`, compose file out of the build context and the image |
| `COPY --chown` instead of `COPY` + `RUN chown` | `COPY --chown=portal:portal . .` | Avoids a second layer that duplicates all copied files |
| Dependencies before source | order of instructions | Cache reuse (speed, not size) |

## 3. The multi-stage variant (`Dockerfile.multistage`)
Stage 1 builds wheels for all dependencies; stage 2 starts from a fresh slim image, installs from those wheels offline and discards stage 1. The idea is that compilers, headers and caches used to build never reach the final image. **Honest expectation:** for *this* application every dependency (Flask, Gunicorn, `psycopg2-binary`) already ships as a pre-built wheel, and `python:3.12-slim` contains no compiler, so there is nothing heavy to leave behind: the multi-stage image is expected to be about the same size as the single-stage one, possibly slightly larger because of the extra `COPY`/`rm` layers. The method pays off when a package must be compiled (for example `psycopg2` instead of `psycopg2-binary`, or `numpy` from source), which would otherwise force `gcc` and `libpq-dev` into the final image. This expectation must be checked with the measurement below, and the result reported even if it shows no gain.

## 4. The full-Ubuntu variant (`Dockerfile.ubuntu`)
Built only for comparison. It installs `python3`, `pip` and `venv` on `ubuntu:24.04` with `--no-install-recommends` and removes the apt lists.

## 5. Measurement table (all cells PENDING)
Commands (also in `capture_evidence.sh`):
```bash
time docker build --no-cache -t result-portal:slim .
time docker build --no-cache -f Dockerfile.multistage -t result-portal:multistage .
time docker build --no-cache -f Dockerfile.ubuntu     -t result-portal:ubuntu .
docker images --format 'table {{.Repository}}:{{.Tag}}\t{{.Size}}' | grep result-portal
docker run --rm --entrypoint sh result-portal:slim -c 'dpkg -l | grep -c ^ii; pip list | wc -l'   # repeat for each tag
docker rmi python:3.12-slim ubuntu:24.04; time docker pull python:3.12-slim; time docker pull ubuntu:24.04
```

| Variant | Image size | Cold build time | Pull time of base image | OS packages (`dpkg`) | Python packages |
|---|---|---|---|---|---|
| Single-stage on `python:3.12-slim` (official) | PENDING | PENDING | PENDING | PENDING | PENDING |
| Multi-stage on `python:3.12-slim` | PENDING | PENDING | same base | PENDING | PENDING |
| Single-stage on `ubuntu:24.04` | PENDING | PENDING | PENDING | PENDING | PENDING |

Pull time to a new server is dominated by the compressed bytes that are not already present; layers shared with an image already on the server are not pulled again, which is why a common base image is efficient across several services.

## 6. How to interpret the results (guide for filling in the report)
- Size ≈ base image + dependency layer + application layer. Compare the **layer** sizes in `docker history`, not only the total.
- Fewer OS packages means fewer places for security problems; a scanner such as `docker scout cves result-portal` or `trivy image result-portal` gives a count per image (optional, PENDING).
- Report build time as cold (`--no-cache`) and warm (change only `app.py`): the warm build demonstrates the layer cache.
