# Workflow: `export_standards_data.yml` — Manual Standards Export

> **File:** `.github/workflows/export_standards_data.yml`  
> **Back to:** [CI/CD Overview](../overview.md)

## Purpose

Manually triggered workflow that downloads the current OpenStudio SDK, runs the Ruby script that generates OpenStudio Standards template `.osm` files, and commits the results back to the repository. This is how the bundled template models in `src/openstudio_app/Resources/` are kept up to date with new SDK releases.

---

## Trigger

```yaml
on:
  workflow_dispatch:
    inputs:
      os_installer_link:
        description: 'URL of the OpenStudio SDK Linux .deb (optional override)'
        required: false
      branch_name:
        description: 'Branch to commit results to (optional, defaults to current)'
        required: false
```

---

## Job

```mermaid
flowchart TD
  A["export-os-standards job\nubuntu-latest"] --> B["Checkout"]
  B --> C["Parse SDK version from FindOpenStudioSDK.cmake\n(inline Python)"]
  C --> D{os_installer_link input set?}
  D -- yes --> DIRECT["Use provided URL"]
  D -- no --> DETECT["Try default URL\nFallback: S3 alternate URL\nFallback: latest GitHub release"]
  DIRECT --> E["Download + install OpenStudio SDK .deb"]
  DETECT --> E
  E --> F[Setup target branch\n(create if not exists)]
  F --> G["Run: N=$(nproc) openstudio export_openstudio_standards_libraries.rb\nin developer/ruby/"]
  G --> H["Copy generated .osm files\nto src/openstudio_app/Resources/"]
  H --> I["Copy export logs"]
  I --> J["git add + git commit + git push\nto target branch"]
```

---

## When to Run

Run this workflow when:
- A new OpenStudio SDK version is released and the template OSM files need to be regenerated
- The `check_osm_versions.yml` gate is failing on the `master` PR
- Structural changes to the standards data require a full regeneration

---

## Inputs

| Input | Required | Default | Description |
|---|---|---|---|
| `os_installer_link` | No | Auto-detected | Direct URL to an OpenStudio SDK `.deb` installer |
| `branch_name` | No | Current branch | Target branch to commit the generated OSM files to |

---

## Outputs

After the workflow runs, it commits to the target branch:
- Updated `.osm` files in `src/openstudio_app/Resources/`
- Export log files from the Ruby script

Create a PR from the target branch to merge the updated OSMs into `develop` or `master`.

---

## Related Files

| File | Role |
|---|---|
| `developer/ruby/export_openstudio_standards_libraries.rb` | Ruby script that generates the OSM files |
| `FindOpenStudioSDK.cmake` | Source of the SDK version number for download URL construction |
| `src/openstudio_app/Resources/*.osm` | Generated template files committed by this workflow |
