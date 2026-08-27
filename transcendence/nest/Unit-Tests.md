┌─────────────────────────────────────────────────────────────┐
│  CONTROLLER TESTS (groups.controller.spec.ts)               │
│  → Testet: HTTP-Endpoints, Request/Response, Guards         │
│  → Mockt: GroupsService (komplett)                          │
└─────────────────────────────────────────────────────────────┘
                              ↑
                              | ruft auf
                              ↓
┌─────────────────────────────────────────────────────────────┐
│  SERVICE TESTS (groups.service.spec.ts)                     │
│  → Testet: Business Logic, DB-Interaktion, Exceptions       │
│  → Mockt: Repositories (GroupRepository, UserGroupRepo)     │
│  → Mockt: Logger                                            │
└─────────────────────────────────────────────────────────────┘
                              ↑
                              | nutzt
                              ↓
┌──────────────────────────────────────────────────────────────┐
│  TEST UTILITIES (test-utils/)                                │
│  → group.factory.ts: Erstellt gültige Group-Objekte          │
│  → mock-repositories.ts: Erstellt typisierte Repository-Mocks│
└──────────────────────────────────────────────────────────────┘