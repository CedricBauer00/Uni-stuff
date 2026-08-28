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


## Parent - Child modules

==**NestJS-Regel**: Alles was in imports des Root-Moduls (AppModule) steht, wird automatisch als Child-Modul registriert. Es gibt keine separate "Child"-Deklaration – der Import ist die Eltern-Kind-Beziehung.==

`"Modules imported in the root module are globally available by default."
— NestJS Docs: Module Re-exporting`

- Konkret: Provider aus Modulen, die direkt im AppModule importiert werden, landen im Root-Container und sind für alle Child-Module sichtbar.

### Tabelle
Frage	Antwort
Wo steht die Parent-Child-Beziehung?	app.module.ts → imports: [GroupsModule]
Woher weißt du, dass Provider global sind?	1. app.get(Logger) in main.ts funktioniert<br>2. NestJS Spec: Root-Imports = globaler Container<br>3. Services in Child-Modulen können per DI darauf zugreifen

## Relevante member functions

### Inhaltsverzeichnis

1. `NestFactory.create()`
2. `app.listen()`
3. `@Module()`
4. `@Injectable()`
5. `@Controller()`
6. HTTP-Methoden-Decorators: `@Get()`, `@Post()`, `@Patch()`, `@Delete()`
7. `@Body()`
8. `@Param()`
9. `@Query()`
10. `@Req()` und `@Res()`
11. `@UseGuards()`
12. `CanActivate` und `canActivate()`
13. `ExecutionContext`
14. `@UsePipes()` und `ValidationPipe`
15. `ParseUUIDPipe`, `ParseIntPipe` und andere Parse-Pipes
16. `@InjectRepository()` und `getRepositoryToken()`
17. `module.get()`
18. `createTestingModule()` und `.compile()`
19. `HttpException` und NestJS-Exceptions
20. `Logger.log()`, `Logger.warn()` und `Logger.error()`

---

## 1. `NestFactory.create()`

### Was macht die Funktion?

`NestFactory.create()` erstellt die laufende NestJS-Anwendung. Dabei wird das Root-Modul, normalerweise `AppModule`, geladen und NestJS baut den Dependency-Injection-Container auf.

### Was gibt man hinein und was kommt zurück?

```typescript
const app = await NestFactory.create(AppModule);
```

- Eingabe: ein Root-Modul, zum Beispiel `AppModule`.
- Rückgabe: ein Promise mit einer NestJS-Application-Instanz.
- `await` wartet, bis die Anwendung erstellt wurde.

### Wann verwendet man sie?

Normalerweise einmal in `main.ts`, beim Starten der Anwendung.

### Warum ist sie relevant?

Ohne diese Funktion wird die NestJS-Anwendung nicht gestartet und Controller werden nicht für HTTP-Anfragen aktiviert.

```typescript
async function bootstrap() {
  const app = await NestFactory.create(AppModule);
  await app.listen(3000);
}

bootstrap();
```

---

## 2. `app.listen()`

### Was macht die Funktion?

`listen()` startet den HTTP-Server und lässt ihn auf einem Port auf Anfragen warten.

### Was gibt man hinein und was kommt zurück?

```typescript
await app.listen(3000);
```

- Eingabe: Portnummer oder Port-Konfiguration.
- Rückgabe: ein Promise, das abgeschlossen ist, wenn der Server gestartet wurde.

### Wann verwendet man sie?

Am Ende der Bootstrap-Funktion in `main.ts`.

### Warum ist sie relevant?

Erst durch `listen()` kann ein Client Endpoints wie `GET /groups` aufrufen.

```typescript
await app.listen(process.env.PORT ?? 3000);
```

---

## 3. `@Module()`

### Was macht der Decorator?

`@Module()` beschreibt die Struktur eines NestJS-Moduls. Er teilt NestJS mit, welche Controller, Provider, Imports und Exports zu diesem Modul gehören.

### Was gibt man hinein und was kommt zurück?

```typescript
@Module({
  imports: [],
  controllers: [],
  providers: [],
  exports: [],
})
export class GroupsModule {}
```

- Eingabe: ein Konfigurationsobjekt.
- Rückgabe: technisch ein Decorator; die Klasse wird mit NestJS-Metadaten versehen.

### Wann verwendet man ihn?

Für jedes NestJS-Modul, zum Beispiel `AppModule` oder `GroupsModule`.

### Wichtige Eigenschaften

| Eigenschaft | Zweck |
|---|---|
| `imports` | Andere Module einbinden |
| `controllers` | Controller dieses Moduls registrieren |
| `providers` | Services und andere Provider registrieren |
| `exports` | Provider für importierende Module verfügbar machen |

```typescript
@Module({
  imports: [TypeOrmModule.forFeature([Group])],
  controllers: [GroupsController],
  providers: [GroupsService],
  exports: [GroupsService],
})
export class GroupsModule {}
```

---

## 4. `@Injectable()`

### Was macht der Decorator?

`@Injectable()` markiert eine Klasse als Provider, den NestJS über Dependency Injection erstellen und in andere Klassen einsetzen darf.

### Was gibt man hinein und was kommt zurück?

```typescript
@Injectable()
export class GroupsService {}
```

- Eingabe: keine Argumente.
- Rückgabe: ein Decorator für die Klasse.

### Wann verwendet man ihn?

Bei Services, Guards, Pipes, Interceptors und anderen Klassen, die NestJS verwalten soll.

### Warum ist er relevant?

Der Controller muss den Service nicht selbst mit `new GroupsService(...)` erstellen. NestJS übernimmt Erstellung und Dependencies.

```typescript
@Injectable()
export class GroupsService {
  constructor(private readonly logger: Logger) {}
}
```

---

## 5. `@Controller()`

### Was macht der Decorator?

`@Controller()` markiert eine Klasse als Controller und legt optional den gemeinsamen URL-Präfix fest.

### Was gibt man hinein und was kommt zurück?

```typescript
@Controller('groups')
export class GroupsController {}
```

- Eingabe: optionaler URL-Präfix, hier `'groups'`.
- Rückgabe: ein Decorator für die Klasse.

### Wann verwendet man ihn?

Bei Klassen, die HTTP-Anfragen entgegennehmen.

```typescript
@Controller('groups')
export class GroupsController {
  // @Get() ergibt später GET /groups
}
```

---

## 6. HTTP-Methoden-Decorators

### Was machen sie?

`@Get()`, `@Post()`, `@Patch()` und `@Delete()` verbinden eine Controller-Methode mit einer HTTP-Methode und optional einem URL-Pfad.

### Was gibt man hinein und was kommt zurück?

```typescript
@Get(':id')
async get(@Param('id') id: string) {
  return this.groupsService.get(id);
}
```

- Eingabe: optionaler Pfad, zum Beispiel `':id'`.
- Rückgabe: ein Method-Decorator; NestJS speichert die Route als Metadaten.

### Wann verwendet man sie?

Für jeden öffentlichen HTTP-Endpoint.

### Wie liest man die Route?

```typescript
@Controller('groups')
@Get(':id')
```

ergibt:

```text
GET /groups/:id
```

```typescript
@Post('create')
async create(@Body() dto: CreateGroupsDto) {}
```

ergibt `POST /groups/create`.

---

## 7. `@Body()`

### Was macht der Decorator?

`@Body()` liest den JSON-Body einer HTTP-Anfrage und gibt ihn als Argument an die Controller-Methode weiter.

### Was gibt man hinein und was kommt zurück?

```typescript
async create(@Body() dto: CreateGroupsDto) {}
```

- Eingabe: optional ein Property-Name, zum Beispiel `@Body('name')`.
- Laufzeitwert: der gesamte Body oder das ausgewählte Property.
- Der Decorator selbst liefert Metadaten für NestJS.

### Wann verwendet man ihn?

Bei `POST`, `PATCH` oder `PUT`, wenn Daten vom Client gesendet werden.

```typescript
@Post()
create(@Body() dto: CreateGroupsDto) {
  return this.groupsService.create(dto);
}
```

---

## 8. `@Param()`

### Was macht der Decorator?

`@Param()` liest Werte aus dem URL-Pfad.

### Was gibt man hinein und was kommt zurück?

```typescript
@Get(':id')
get(@Param('id') id: string) {}
```

- Eingabe: Name des URL-Parameters, hier `'id'`.
- Laufzeitwert: der Wert aus der URL, zum Beispiel `'abc-123'`.

### Wann verwendet man ihn?

Wenn ein Endpoint eine Ressource über ihre ID oder einen anderen Pfadwert auswählt.

```text
GET /groups/abc-123
```

Dann enthält `id` den Wert `'abc-123'`.

---

## 9. `@Query()`

### Was macht der Decorator?

`@Query()` liest Query-Parameter aus der URL.

### Was gibt man hinein und was kommt zurück?

```typescript
@Get()
findAll(@Query('page') page: string) {}
```

- Eingabe: optional der Name eines Query-Parameters.
- Laufzeitwert: der Wert aus der URL.

```text
GET /groups?page=2
```

Hier wird `page` als `'2'` gelesen. URL-Werte sind zunächst Strings und müssen bei Bedarf in Zahlen umgewandelt werden.

---

## 10. `@Req()` und `@Res()`

### Was machen sie?

`@Req()` gibt Zugriff auf das Express-Request-Objekt. `@Res()` gibt Zugriff auf das Response-Objekt.

### Was gibt man hinein und was kommt zurück?

```typescript
async remove(@Param('id') id: string, @Req() req: Request) {
  return this.groupsService.deleteGroup(id, req.userId!, false);
}
```

- `@Req()` nimmt normalerweise keine Argumente.
- Es liefert zur Laufzeit das aktuelle Request-Objekt.
- `@Res()` liefert das Response-Objekt.

### Wann verwendet man sie?

`@Req()` zum Lesen von Session-, User- oder Header-Daten. `@Res()` nur, wenn man die HTTP-Antwort manuell steuern muss.

```typescript
@Get('me')
getMe(@Req() req: Request) {
  return { userId: req.userId };
}
```

In NestJS sollte man normalerweise einfach einen Wert zurückgeben und `@Res()` vermeiden, weil NestJS dann Status und JSON-Antwort automatisch verwaltet.

---

## 11. `@UseGuards()`

### Was macht der Decorator?

`@UseGuards()` schützt einen Controller oder eine einzelne Methode mit einem oder mehreren Guards.

### Was gibt man hinein und was kommt zurück?

```typescript
@Get(':id')
@UseGuards(SessionAuthGuard)
get(@Param('id') id: string) {}
```

- Eingabe: eine oder mehrere Guard-Klassen beziehungsweise Guard-Instanzen.
- Rückgabe: ein Decorator.

### Wann verwendet man ihn?

Wenn nur authentifizierte oder berechtigte Nutzer einen Endpoint verwenden dürfen.

```typescript
@Controller('groups')
@UseGuards(SessionAuthGuard)
export class GroupsController {}
```

Dann gilt der Guard für alle Methoden des Controllers.

---

## 12. `CanActivate` und `canActivate()`

### Was macht `canActivate()`?

Ein Guard entscheidet, ob eine Anfrage weiterlaufen darf.

### Was gibt man hinein und was kommt zurück?

```typescript
canActivate(context: ExecutionContext): boolean {
  return true;
}
```

- Eingabe: `ExecutionContext` mit Informationen über die aktuelle Anfrage.
- Rückgabe: `boolean` oder auch `Promise<boolean>` beziehungsweise `Observable<boolean>`.
- `true`: Anfrage darf weiterlaufen.
- `false`: Zugriff wird verweigert.

### Wann verwendet man sie?

Für Authentifizierung und Autorisierung.

```typescript
@Injectable()
export class SessionAuthGuard implements CanActivate {
  canActivate(context: ExecutionContext): boolean {
    const request = context.switchToHttp().getRequest<Request>();
    return Boolean(request.userId);
  }
}
```

Ein Guard sollte nicht nur prüfen, ob ein Wert existiert, sondern die Session oder Berechtigung fachlich korrekt validieren.

---

## 13. `ExecutionContext`

### Was macht die Klasse beziehungsweise das Interface?

`ExecutionContext` enthält Informationen darüber, in welchem Kontext NestJS gerade arbeitet, zum Beispiel HTTP, WebSocket oder RPC.

### Wichtige Methoden

```typescript
const request = context.switchToHttp().getRequest<Request>();
```

| Methode | Bedeutung |
|---|---|
| `switchToHttp()` | Wechselt zum HTTP-Kontext |
| `getRequest()` | Holt das aktuelle Request-Objekt |
| `getResponse()` | Holt das aktuelle Response-Objekt |
| `getHandler()` | Holt die aktuelle Controller-Methode |
| `getClass()` | Holt die aktuelle Controller-Klasse |

### Wann verwendet man sie?

Vor allem in Guards, Interceptors, Filters und Decorators.

---

## 14. `@UsePipes()` und `ValidationPipe`

### Was machen sie?

Pipes verändern oder validieren Daten, bevor sie die Controller-Methode erreichen.

### Was gibt man hinein und was kommt zurück?

```typescript
@UsePipes(new ValidationPipe({ whitelist: true }))
@Post()
create(@Body() dto: CreateGroupsDto) {}
```

- Eingabe: eine Pipe, zum Beispiel `new ValidationPipe(...)`.
- Rückgabe: ein Decorator beziehungsweise validierte und transformierte Eingabedaten zur Laufzeit.

### Wann verwendet man sie?

Bei DTO-Validierung oder bei der Umwandlung von Request-Strings in typisierte Werte.

```typescript
app.useGlobalPipes(
  new ValidationPipe({ whitelist: true, transform: true }),
);
```

`whitelist: true` entfernt Felder, die nicht im DTO definiert sind. `transform: true` erlaubt typisierte Umwandlungen, wenn DTOs und Decorators passend definiert sind.

---

## 15. Parse-Pipes

### Was machen sie?

Parse-Pipes prüfen und konvertieren einzelne Parameter.

### Was gibt man hinein und was kommt zurück?

```typescript
@Get(':id')
get(@Param('id', new ParseUUIDPipe()) id: string) {}
```

- Eingabe: den Parameterwert aus dem Request.
- Rückgabe: den geprüften beziehungsweise konvertierten Wert.
- Bei ungültigen Werten wirft NestJS eine HTTP-Exception.

### Häufige Beispiele

```typescript
@Param('id', new ParseUUIDPipe()) id: string
@Param('page', new ParseIntPipe()) page: number
```

`ParseUUIDPipe` akzeptiert nur gültige UUIDs. `ParseIntPipe` wandelt einen String wie `'2'` in die Zahl `2` um.

---

## 16. `@InjectRepository()` und `getRepositoryToken()`

### `@InjectRepository()`

Der Decorator weist NestJS an, ein TypeORM-Repository in den Constructor zu injizieren.

```typescript
constructor(
  @InjectRepository(Group)
  private readonly groupsRepository: Repository<Group>,
) {}
```

- Eingabe: die Entity-Klasse, hier `Group`.
- Laufzeitwert: ein Repository für diese Entity.

### `getRepositoryToken()`

Diese Funktion erzeugt den DI-Token für dasselbe Repository, besonders nützlich in Tests.

```typescript
provide: getRepositoryToken(Group)
```

- Eingabe: Entity-Klasse.
- Rückgabe: der Token, unter dem NestJS das Repository registriert.

Im Test kann man damit das echte Repository ersetzen:

```typescript
{
  provide: getRepositoryToken(Group),
  useValue: { findOneBy: jest.fn() },
}
```

---

## 17. `module.get()`

### Was macht die Methode?

`module.get()` holt einen Provider oder Controller aus dem NestJS-Dependency-Injection-Container.

### Was gibt man hinein und was kommt zurück?

```typescript
const service = module.get(GroupsService);
```

- Eingabe: ein DI-Token, oft eine Klasse.
- Rückgabe: die registrierte Instanz.

### Wann verwendet man sie?

In Tests oder innerhalb speziell gestarteter NestJS-Anwendungen.

```typescript
controller = module.get(GroupsController);
service = module.get(GroupsService);
```

Im Controller-Test kann `service` dabei ein Mock sein, während `controller` die echte Controller-Instanz ist.

---

## 18. `createTestingModule()` und `.compile()`

### Was machen sie?

`Test.createTestingModule()` erstellt einen Test-Dependency-Container. `.compile()` baut ihn auf und löst Dependencies auf.

### Was gibt man hinein und was kommt zurück?

```typescript
const module = await Test.createTestingModule({
  controllers: [GroupsController],
  providers: [
    {
      provide: GroupsService,
      useValue: { create: jest.fn() },
    },
  ],
}).compile();
```

- Eingabe: Test-Konfiguration mit `controllers`, `providers` und eventuell `imports`.
- `createTestingModule()` gibt einen Builder zurück.
- `.compile()` gibt ein `Promise<TestingModule>` zurück.

### Warum verwendet man sie?

Damit ein echter Controller isoliert mit einem gemockten Service getestet werden kann, ohne Datenbank oder komplette Anwendung zu starten.

---

## 19. `HttpException` und NestJS-Exceptions

### Was machen sie?

Exceptions teilen NestJS mit, dass eine Anfrage mit einem bestimmten HTTP-Fehler beendet werden soll.

### Was gibt man hinein und was kommt zurück?

```typescript
throw new NotFoundException('Group not found');
```

- Eingabe: optional Fehlermeldung und Statusinformationen.
- Rückgabe: ein Exception-Objekt; mit `throw` wird es ausgelöst.
- NestJS wandelt es normalerweise in eine HTTP-Fehlerantwort um.

### Häufige Exceptions

```typescript
throw new NotFoundException();       // 404
throw new ForbiddenException();      // 403
throw new ConflictException();       // 409
throw new BadRequestException();     // 400
throw new UnauthorizedException();   // 401
```

### Wann verwendet man sie?

Wenn die Business-Logik einen Fehler feststellt. Im Unit-Test prüft man sie zum Beispiel so:

```typescript
await expect(service.get('missing-id'))
  .rejects.toThrow(NotFoundException);
```

---

## 20. `Logger.log()`, `Logger.warn()` und `Logger.error()`

### Was machen die Methoden?

Sie schreiben Informationen, Warnungen oder Fehler in das Logging-System.

### Was gibt man hinein und was kommt zurück?

```typescript
this.logger.log({ groupId: id }, 'Fetching group');
this.logger.warn({ groupId: id }, 'Group not found');
this.logger.error(error, 'Group loading failed');
```

- Eingabe: Nachricht und optional strukturierte Daten oder ein Fehlerobjekt.
- Rückgabe: normalerweise nichts Relevantes (`void`).

### Wann verwendet man sie?

Für Nachvollziehbarkeit und Fehlersuche im laufenden Server.

### Im Unit-Test

Der Logger wird meist gemockt, weil man nicht die Logging-Bibliothek testen möchte:

```typescript
{
  provide: Logger,
  useValue: {
    log: jest.fn(),
    warn: jest.fn(),
    error: jest.fn(),
  },
}
```

## Gesamtbild: Wie NestJS die Teile verbindet

```text
NestFactory.create(AppModule)
        |
        v
AppModule imports GroupsModule
        |
        v
GroupsModule registriert Controller und Services
        |
        v
@Controller('groups')
        |
        v
@Get(':id') liest @Param('id')
        |
        v
GroupsController ruft GroupsService auf
        |
        v
GroupsService verwendet Repository und Logger
        |
        v
Service gibt DTO zurück oder wirft eine Exception
```

## Kurze Lernformel

```text
Module organisieren.
Controller empfangen Requests.
Decorators beschreiben Routes und Parameter.
Guards entscheiden über Zugriff.
Pipes validieren und transformieren Eingaben.
Services enthalten Business-Logik.
Repositories sprechen mit der Datenbank.
Exceptions erzeugen HTTP-Fehler.
NestJS Dependency Injection verbindet alles.
```