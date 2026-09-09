# Questions

## 1. Das `!` (Non-Null Assertion Operator)

```ts
req.userId!  // "TypeScript, ich verspreche: das ist NICHT null/undefined"
```

### Tabelle

| Ohne `!` | Mit `!` |
|---|---|
| `req.userId` ist `string \| undefined` | `req.userId` ist `string` |
| TypeScript warnt: „kann `undefined` sein“ | TypeScript vertraut dir: „ist definitiv `string`“ |
| Compile Error, wenn `string` erwartet wird | Kein Error |

---

## 2. Warum `AuthenticatedRequest` funktioniert

```ts
interface AuthenticatedRequest extends Request {
  userId: string;  // ← HIER: required, nicht optional!
}

@Req() req: AuthenticatedRequest  // TypeScript denkt: userId ist IMMER string
```

### Tabelle

| Definition | TypeScript-Typ |
|---|---|
| `interface Request { userId?: string }` | `string \| undefined` — optional |
| `interface AuthenticatedRequest { userId: string }` | `string` — required |

### Wie TypeScript die verschiedenen Definitionen betrachtet

```ts
// 1. Standard Request (express.d.ts)

interface Request {
  userId?: string;  // optional
}

// 2. Module Augmentation (deine express.d.ts)

declare global {
  namespace Express {
    interface Request {
      userId?: string;  // immer noch optional!
    }
  }
}

// 3. Dein Custom Interface (lügt TypeScript an)

interface AuthenticatedRequest extends Request {
  userId: string;  // überschreibt optional → required
}
```

| Schritt | Definition | Ergebnis |
|---|---|---|
| **1. Standard Request** | `interface Request { userId?: string }` | `userId` ist optional → `string \| undefined` |
| **2. Module Augmentation** | `interface Request { userId?: string }` | `userId` bleibt optional → `string \| undefined` |
| **3. Custom Interface** | `interface AuthenticatedRequest extends Request { userId: string }` | `userId` wird required → `string` |

---

## 3. `Request & { userId: string }` – Intersection Type

```ts
@Req() req: Request & { userId: string }
```

### Intersection Type (Schnittmenge)

- `Request` = alle Standard-Express-Properties (`headers`, `body`, `params`, `query`, `cookies`, etc.)
- `{ userId: string }` = zusätzliche Property
- Ergebnis = ein Typ, der **beides** hat

Äquivalent zu:

```ts
interface MyRequest extends Request {
  userId: string;
}
```

> ⚠️ **Wichtig:** Bei jeder Function Declaration notwendig – es muss bei jedem Parameter wiederholt werden.

### Beispiel

```ts
// Controller 1
@Get()
async findAll(@Req() req: Request & { userId: string }) { ... }

// Controller 2
@Post()
async create(@Req() req: Request & { userId: string }) { ... }

// Controller 3
@Delete()
async remove(@Req() req: Request & { userId: string }) { ... }

// Service (falls du Request weiterreichst)
async doSomething(req: Request & { userId: string }) { ... }
```

---

## 5. (Fix) Module Augmentation

> 💡 **Einmalige Declaration**

```ts
// src/types/express.d.ts (EINMAL anlegen)

import { Request } from 'express';

declare global {
  namespace Express {
    interface Request {
      userId?: string; // optional, global für ALLE Requests
    }
  }
}
```

```json
// tsconfig.json

{
  "compilerOptions": {
    "typeRoots": ["./src/types", "./node_modules/@types"]
  },
  "include": ["src/**/*", "src/types/**/*"]
}
```

### Danach einfach überall `Request`

```ts
// Controller 1
@Get()
async findAll(@Req() req: Request) {
  req.userId  // ✅
}

// Controller 2
@Post()
async create(@Req() req: Request) {
  req.userId  // ✅
}

// Service
async doSomething(req: Request) {
  req.userId  // ✅
}

// Guard
canActivate(context) {
  const req = context.switchToHttp().getRequest<Request>();
  req.userId  // ✅
}
```

---

## 6. Was ist `Request` – Interface oder Klasse?

**Klasse ≠ Interface**

`Request` ist ein **Interface**:

```ts
// In @types/express (node_modules/@types/express/index.d.ts)

interface Request<
  P = ParamsDictionary,
  ResBody = any,
  ReqBody = any,
  ReqQuery = Query
> extends core.Request<P, ResBody, ReqBody, ReqQuery> {

  // Properties:
  body: ReqBody;
  params: P;
  query: ReqQuery;
  headers: any;
  cookies: any;
  method: string;
  url: string;
  path: string;

  // ... und viele mehr
}
```

### Tabelle

| Aspekt | Erklärung |
|---|---|
| **Typ** | Interface (keine Klasse!) |
| **Herkunft** | `@types/express` – TypeScript-Definitionen |
| **Runtime** | Express erstellt ein Plain Object und fügt Properties hinzu |
| **Instanz** | `new Request()` gibt es nicht – Express baut das Request-Objekt intern |

---

## 7. `!!` Operator

Der `!!` Operator wandelt einen Wert in einen `boolean` um.

```ts
const membership = await this.userGroupsRepository.findOneBy({
  userId,
  groupId
});

// membership: UserGroup | null

!!membership        // true / false
Boolean(membership) // true / false
```

### `!!membership` = `Boolean(membership)`

Beide machen dasselbe:

```ts
!!membership
Boolean(membership)
```

### Funktionsweise

```ts
// Erster ! : negiert zu boolean
// true → false
// false → true

// Zweiter ! : negiert zurück
// Ergebnis ist wieder true/false

!!null        // false
!!undefined   // false
!!{}          // true
!!{ id: 1 }   // true
!!0           // false
!!"hello"     // true
```

### Step by Step

| Schritt | Code | Ergebnis | Typ |
|---|---|---|---|
| **Original** | `membership` | `{ id: "..." }` oder `null` | `UserGroup \| null` |
| **1. `!`** | `!membership` | `false` (wenn Objekt) / `true` (wenn `null`) | `boolean` |
| **2. `!!`** | `!!membership` | `true` (wenn Objekt) / `false` (wenn `null`) | `boolean` |

### Beispiele

| Wert | `!!Wert` |
|---|---:|
| `null` | `false` |
| `undefined` | `false` |
| `{}` | `true` |
| `{ id: 1 }` | `true` |
| `0` | `false` |
| `"hello"` | `true` |

---

# Parent – Child Modules

> **NestJS-Regel:** Alles, was in `imports` des Root-Moduls (`AppModule`) steht, wird automatisch als Child-Modul registriert. Es gibt keine separate „Child“-Deklaration – der Import ist die Eltern-Kind-Beziehung.

> „Modules imported in the root module are globally available by default.“
>
> — NestJS Docs: Module Re-exporting

### Konkret

Provider aus Modulen, die direkt im `AppModule` importiert werden, landen im Root-Container und sind für alle Child-Module sichtbar.

### Tabelle

| Frage | Antwort |
|---|---|
| **Wo steht die Parent-Child-Beziehung?** | `app.module.ts` → `imports: [GroupsModule]` |
| **Woher weißt du, dass Provider global verfügbar sind?** | 1. `app.get(Logger)` in `main.ts` funktioniert.<br>2. NestJS definiert den Root-Container entsprechend.<br>3. Services in Child-Modulen können per DI darauf zugreifen. |
