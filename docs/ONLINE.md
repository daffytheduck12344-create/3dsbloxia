# Online multiplayer plan

**Status: not implemented.** The current native prototype is local single-player. This document describes a possible future implementation; it is not a claim that the game currently connects online.

## Proposed approach

1. Keep the game simulation and round rules deterministic and separate from rendering/input.
2. Add a small, independently hosted relay/matchmaking service using a documented message protocol. The service, not an individual console, owns authoritative roles, round timers, player positions, and hit/evidence validation.
3. Use short-lived room codes and random session tokens. Never collect Nintendo Network credentials, Roblox credentials, or account passwords. Avoid storing personal data.
4. Use authenticated, encrypted transport where the 3DS networking stack and chosen service can support it. Validate every packet's size, version, player/room membership, and sequence; rate-limit requests and reject malformed input.
5. Add explicit connect/disconnect states, timeout/reconnect handling, and offline fallback. Test with a private test server before exposing anything publicly.
6. Document server deployment, protocol versioning, privacy, and moderation before enabling public matchmaking.

## 3DS-specific constraint

A homebrew application needs network initialization and compatible socket/TLS libraries, plus a reachable service endpoint. Availability and compatibility depend on the devkitPro/libctru version and the user's network. No production endpoint, certificates, or credentials are bundled here. Until those pieces are designed, tested, and reviewed, online mode stays disabled rather than pretending a local game is online.
