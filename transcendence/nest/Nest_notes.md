# Questions

# 1. Das ! (Non-Null Assertion Operator) 

```
req.userId!  // "TypeScript, ich verspreche: das ist NICHT null/undefined"
```
## Tabelle
Ohne !	Mit !
req.userId ist string | undefined	req.userId ist string
TypeScript warnt: "kann undefined sein"	TypeScript vertraut dir: "ist definitiv string"
Compile Error wenn string erwartet	Kein Error

# 2. Warum AuthenticatedRequest funktionierte

```
interface AuthenticatedRequest extends Request {
  userId: string;  // ← HIER: **required**, nicht optional!
}

@Req() req: AuthenticatedRequest  // TypeScript denkt: userId ist IMMER string
```
## Tabelle
Definition	TypeScript Typ
interface Request { userId?: string }	string | undefined (optional)
interface AuthenticatedRequest { userId: string }	string (required)

```
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
# 3.  Request & { userId: string } – Intersection Type

```
@Req() req: Request & { userId: string }
```
- Intersectopm Type (Schnittmenge):
  - Request = alle Standard-Express-Properties ( headers, body, params, query, cookies, etc.)
  - { userId: string } = zusaetzliche Property
  - ErgebnisL Ein Typ der beides jat
    - Aequivalent zu: 
    ```
    interface MyRequest extends Request {
      userId: string;
    }
    ```
==!Bei jeder function declaration notwendig - es muss bei jedem Parameter wiederholt werden!==
## Bsp
```
// Controller 1
@Get() async findAll(@Req() req: Request & { userId: string }) { ... }

// Controller 2  
@Post() async create(@Req() req: Request & { userId: string }) { ... }

// Controller 3
@Delete() async remove(@Req() req: Request & { userId: string }) { ... }

// Service (falls du Request weiterreichst)
async doSomething(req: Request & { userId: string }) { ... }
```

# 5. (Fix) Module Augmentation 

==!einmalige declaration!==
```
// src/types/express.d.ts (EINMAL anlegen)
import { Request } from 'express';

declare global {
  namespace Express {
    interface Request {
      userId?: string; // optional, global fuer ALLE Requests
    }
  }
}
```
```
// tsconfig.json
{
  "compilerOptions": {
    "typeRoots": ["./src/types", "./node_modules/@types"]
  },
  "include": ["src/**/*", "src/types/**/*"]
}
```
### Danach einfach ueberall `Request`
```
// Controller 1
@Get() async findAll(@Req() req: Request) { req.userId }  // ✅

// Controller 2
@Post() async create(@Req() req: Request) { req.userId }  // ✅

// Service
async doSomething(req: Request) { req.userId }  // ✅

// Guard
canActivate(context) {
  const req = context.switchToHttp().getRequest<Request>();
  req.userId  // ✅
}
```

# 6. Was ist Request? Interface || Klasse ?

- Klasse != Interface

Request:
```
// In @types/express (node_modules/@types/express/index.d.ts)
interface Request<P = ParamsDictionary, ResBody = any, ReqBody = any, ReqQuery = Query> 
  extends core.Request<P, ResBody, ReqBody, ReqQuery> {
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

## Tabelle
Aspekt	Erklärung
Typ	Interface (keine Klasse!)
Herkunft	@types/express (TypeScript Definitionen)
Runtime	Express erstellt ein Plain Object und fügt Properties hinzu
Keine Instanz	new Request() gibt's nicht – Express baut es intern

# 7. !! Operator

- wandelt in boolean um...
- ==!!membership == Boolean(membership)==
- beide machen dasselbe
```
const membership = await this.userGroupsRepository.findOneBy({userId, groupId});
// membership: UserGroup | null

!!membership        // true / false
Boolean(membership) // true / false
```

## Funktionsweise
```
// Erster ! : negiert zu boolean (true → false, false → true)
// Zweiter ! : negiert zurück (wieder true/false)

!!null        // false
!!undefined   // false
!!{}          // true
!!{ id: 1 }   // true
!!0           // false
!!"hello"     // true
```

### Step by step
## Tabelle
Schritt	Code	Ergebnis	Typ
Original	membership	{ id: "..." } oder null	UserGroup \| null
1. !	!membership	false (wenn Objekt) / true (wenn null)	boolean
2. !!	!!membership	true (wenn Objekt) / false (wenn null)	boolean


