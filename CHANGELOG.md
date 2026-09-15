# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/).

## [Unreleased]

## [1.0.0] - 2026-09-15

### Added
- Initial release of VeinMiner
- Flood-fill vein breaking for connected same-type blocks
- Configurable activation modes: sneak, always active, toggle command
- Server-wide settings via `/miner config` for operators
- Personal settings via `/miner config` for regular players
- `/miner reload` to reload configuration from disk without restarting
- Configurable block whitelist, max blocks per vein, and max search radius
- Configurable base-use permission default (everyone or operators only)
