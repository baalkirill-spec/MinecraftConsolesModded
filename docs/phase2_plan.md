# Phase 2 stabilization plan

## 1. Chunk distance UI polishing
- Audit current render distance naming vs. actual chunk radius mapping.
- Surface the effective chunk count / real distance in UI text so players understand cost.
- Keep the existing option storage compatible with current saves/settings.

## 2. Camera bobbing and run behavior
- Compare current bobbing against vanilla-like walk/run timing and amplitude.
- Verify flying, ladders, swimming, hurt camera, and split-screen edge cases.
- Make changes behind existing settings without breaking current FOV/custom options.

## 3. Safe graphics improvements
- Review clouds, gamma, ambient occlusion, and chunk rebuild side effects first.
- Prefer low-risk improvements like cleaner defaults, safer toggles, and better UI exposure.
- Avoid fake shadow pipeline work until renderer architecture is ready.

## 4. Modding architecture follow-up
- Expand folder-based asset override coverage first.
- Add data-driven content discovery points before registries.
- Design registries/hooks for blocks, items, NPCs, and mechanics only after the phase-1 file/discovery path is stable.
