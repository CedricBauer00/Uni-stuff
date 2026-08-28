# TypeScript, Jest und NestJS Unit Tests

Diese Zusammenfassung erklärt die wichtigsten TypeScript-Grundlagen aus dem Beispiel mit `GroupsController` und `GroupsService` und zeigt, wie man Controller- und Service-Unit-Tests schreibt.

## 1. Grundidee von Unit Tests

Ein Unit Test prüft eine kleine, abgegrenzte Einheit des Codes. Eine Einheit kann zum Beispiel eine einzelne Methode eines Services oder eine Controller-Methode sein.

Unit Tests sind nützlich für:

- **Regressionsschutz:** Änderungen brechen vorhandenes Verhalten nicht unbemerkt.
- **Dokumentation:** Der Test zeigt, welches Verhalten erwartet wird.
- **Refactoring:** Code kann umgebaut werden, solange die Tests weiter funktionieren.
- **Fehlererkennung:** Fehler werden nahe an ihrer Ursache gefunden.

Ein Unit Test verwendet normalerweise keine echte Datenbank. Abhängigkeiten wie Repositories oder Services werden durch Mocks ersetzt.

### 1.1 Ablauf (grob)

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

## 2. Controller- und Service-Tests

### Controller-Test

Ein Controller nimmt HTTP-Eingaben entgegen und ruft normalerweise einen Service auf.

```text
HTTP Request -> Controller -> Service -> Datenbank
```

Im Controller-Unit-Test wird der Controller echt verwendet, der Service aber gemockt:

```text
echter Controller + gemockter Service
```

Man prüft hauptsächlich:

- Werden Parameter richtig aus Body, URL oder Request gelesen?
- Wird die richtige Service-Methode aufgerufen?
- Werden die richtigen Argumente weitergegeben?
- Wird der Rückgabewert des Services zurückgegeben?

### Service-Test

Der Service enthält die Business-Logik. Im Service-Unit-Test wird der Service echt verwendet, seine Abhängigkeiten werden aber gemockt:

```text
echter Service + gemockte Repositories + gemockter Logger
```

Man prüft hauptsächlich:

- Werden die richtigen Datenbankmethoden aufgerufen?
- Werden Regeln korrekt umgesetzt?
- Werden DTOs korrekt erzeugt?
- Werden Exceptions im richtigen Fall ausgelöst?

Merksatz:

> Der Controller-Test prüft die Weiterleitung. Der Service-Test prüft die Logik.

## 3. TypeScript-Grundlagen

### 3.1 Variablen mit `let` und `const`

```typescript
let controller: GroupsController;
const dto = { name: 'A', description: 'B' };
```

`let` erstellt eine Variable, deren Wert später geändert werden kann:

```typescript
let value = 1;
value = 2;
```

`const` erstellt eine Variable, die nicht neu zugewiesen werden darf:

```typescript
const value = 1;
// value = 2; // Fehler
```

Bei Objekten bedeutet `const` allerdings nur, dass die Variable nicht auf ein anderes Objekt zeigen darf. Eigenschaften können grundsätzlich weiterhin verändert werden.

```typescript
const user = { name: 'A' };
user.name = 'B';
```

### 3.2 Type Annotation

```typescript
let controller: GroupsController;
```

Der Teil nach dem Doppelpunkt ist eine Typangabe. Sie sagt TypeScript, welche Art von Wert die Variable enthalten soll.

```typescript
let name: string = 'Test';
let count: number = 3;
let active: boolean = true;
```

Bei `let controller: GroupsController;` wird die Variable zuerst nur deklariert. Der Wert wird später im `beforeEach` gesetzt.

### 3.3 Objekte und Object Literals

Ein Object Literal ist ein direkt geschriebenes Objekt:

```typescript
const dto = {
	name: 'A',
	description: 'B',
};
```

`name` und `description` sind Eigenschaften. Ihre Werte sind `'A'` und `'B'`.

TypeORM verwendet den Typ `ObjectLiteral` als Einschränkung für Repository-Entities. Er bedeutet hier vereinfacht: Der generische Typ soll ein Objekt sein, das Eigenschaften besitzt.

```typescript
Repository<T extends ObjectLiteral>
```

Darum braucht eine generische Mock-Repository-Funktion:

```typescript
export const createMockRepository = <T extends ObjectLiteral>(
	entity: new () => T,
): jest.Mocked<Repository<T>> => {
	// ...
};
```

`T extends ObjectLiteral` bedeutet: `T` darf nur ein gültiger Objekt-Typ sein.

### 3.4 Generics

Ein Generic ist ein Platzhalter für einen Typ:

```typescript
Repository<Group>
Repository<UserGroup>
```

Dasselbe Repository-Konzept kann für unterschiedliche Entities verwendet werden. `T` wird beim Aufruf durch den konkreten Typ ersetzt.

```typescript
createMockRepository(Group);     // T ist Group
createMockRepository(UserGroup); // T ist UserGroup
```

### 3.5 Arrow Functions

```typescript
() => {
	// Code
}
```

Das ist eine Arrow Function, also eine kurze Schreibweise für eine Funktion.

```typescript
const add = (a: number, b: number): number => {
	return a + b;
};
```

Die leeren Klammern `()` bedeuten, dass die Funktion keine Argumente erhält. Mit Argumenten sieht sie so aus:

```typescript
(name: string) => {
	return name;
}
```

In Jest werden Arrow Functions oft als Callback übergeben:

```typescript
describe('GroupsController', () => {
	// Dieser Code wird von describe ausgeführt bzw. registriert.
});
```

### 3.6 Spread Operator `...`

Der Spread Operator kopiert Eigenschaften eines Objekts in ein neues Objekt:

```typescript
const dto = { name: 'A', description: 'B' };
const expected = { id: '1', ...dto };
```

Das Ergebnis ist:

```typescript
{
	id: '1',
	name: 'A',
	description: 'B',
}
```

Ohne Spread wäre es ausgeschrieben:

```typescript
const expected = {
	id: '1',
	name: dto.name,
	description: dto.description,
};
```

Die Reihenfolge ist wichtig, wenn Eigenschaften doppelt vorkommen:

```typescript
const result = { name: 'Old', ...dto };
```

Wenn `dto.name` vorhanden ist, überschreibt es `'Old'`.

### 3.7 `async` und `await`

Eine `async`-Funktion gibt immer ein Promise zurück:

```typescript
async function getName() {
	return 'Test';
}
```

Mit `await` wartet man auf das Ergebnis eines Promises:

```typescript
const name = await getName();
```

Tests werden häufig als `async` geschrieben, weil Controller und Services Datenbankoperationen ausführen und deshalb Promises zurückgeben.

## 4. Jest-Grundlagen

Jest ist das Test-Framework. Es stellt Funktionen wie `describe`, `it`, `expect` und `jest.fn` bereit.

### 4.1 `describe`

```typescript
describe('GroupsController', () => {
	// Tests für GroupsController
});
```

`describe` erstellt eine Test-Suite, also eine Gruppe zusammengehöriger Tests. Der String ist nur eine beschreibende Überschrift im Test-Output.

Man kann Suites verschachteln:

```typescript
describe('GroupsService', () => {
	describe('create()', () => {
		// Tests für create
	});
});
```

### 4.2 `it`

```typescript
it('should create a group', async () => {
	// einzelner Test
});
```

`it` beschreibt einen einzelnen Test. `test` ist ein Alias für `it`.

Der erste Parameter ist der Name. Der zweite Parameter ist eine Funktion mit der eigentlichen Testlogik.

### 4.3 `beforeEach`

```typescript
beforeEach(async () => {
	// Vorbereitung
});
```

`beforeEach` ist ein Hook. Der enthaltene Code wird vor jedem einzelnen `it`-Test ausgeführt.

Das ist wichtig, damit jeder Test eine frische Testumgebung erhält. Ein Test soll nicht von einem vorherigen Test abhängen.

### 4.4 `expect`

```typescript
expect(result).toEqual(expected);
```

`expect` startet eine Prüfung. Der Matcher danach legt fest, was geprüft werden soll.

Häufige Matcher:

```typescript
expect(value).toBe(3);                    // primitive Werte
expect(object).toEqual(expectedObject);   // Inhalte von Objekten
expect(value).toBeDefined();              // nicht undefined
expect(array).toHaveLength(2);             // Array-Länge
expect(fn).toHaveBeenCalled();             // Funktion wurde aufgerufen
expect(fn).toHaveBeenCalledWith(value);    // mit diesem Argument aufgerufen
expect(fn).not.toHaveBeenCalled();         // nicht aufgerufen
```

### 4.5 `jest.fn()`

```typescript
const mockFunction = jest.fn();
```

`jest.fn()` erstellt eine künstliche Funktion, einen sogenannten Mock.

Diese Funktion hat zunächst keine echte Logik, aber Jest zeichnet auf:

- ob sie aufgerufen wurde,
- wie oft sie aufgerufen wurde,
- mit welchen Argumenten sie aufgerufen wurde,
- welchen Wert sie zurückgegeben hat.

Beispiel:

```typescript
const fn = jest.fn();
fn('hello');

expect(fn).toHaveBeenCalledWith('hello');
```

### 4.6 Mock-Rückgabewerte

Für synchrone Funktionen:

```typescript
mockFn.mockReturnValue(value);
```

Für asynchrone Funktionen:

```typescript
mockFn.mockResolvedValue(value);
```

`mockResolvedValue(value)` entspricht ungefähr:

```typescript
mockFn.mockReturnValue(Promise.resolve(value));
```

Für einen asynchronen Fehler:

```typescript
mockFn.mockRejectedValue(new Error('Fehler'));
```

TypeORMs `repository.create()` ist synchron, deshalb verwendet man dafür `mockReturnValue`. Methoden wie `save`, `find` oder `findOneBy` liefern Promises, deshalb verwendet man dafür normalerweise `mockResolvedValue`.

### 4.7 `jest.Mocked<T>`

```typescript
let service: jest.Mocked<GroupsService>;
```

`jest.Mocked<T>` ist ein TypeScript-Hilfstyp von Jest. Er sagt TypeScript, dass die Methoden von `T` gemockte Funktionen sind.

Dadurch erkennt TypeScript zum Beispiel:

```typescript
service.create.mockResolvedValue(expected);
```

Wichtig: `jest.Mocked<GroupsService>` erstellt selbst noch keinen Mock. Es ist nur eine Typbeschreibung. Der echte Mock entsteht in `useValue` durch `jest.fn()`.

## 5. NestJS Testing Module

### 5.1 Import

```typescript
import { Test, TestingModule } from '@nestjs/testing';
```

`Test` stellt Funktionen zum Erstellen einer isolierten NestJS-Testumgebung bereit. `TestingModule` ist der Typ des kompilierten Testmoduls.

### 5.2 `Test.createTestingModule`

```typescript
const module: TestingModule = await Test.createTestingModule({
	controllers: [GroupsController],
	providers: [
		// Dependencies
	],
}).compile();
```

`createTestingModule` erstellt die Konfiguration für einen kleinen NestJS Dependency-Injection-Container.

Das Testmodul ist ähnlich wie ein normales NestJS-Modul, aber auf den Test begrenzt. Man gibt nur die Controller, Services und Mocks an, die benötigt werden.

### 5.3 `controllers: []`

```typescript
controllers: [GroupsController]
```

Damit wird der echte `GroupsController` in das Testmodul aufgenommen. Genau diese echte Instanz wird später getestet.

### 5.4 `providers: []`

```typescript
providers: [
	{
		provide: GroupsService,
		useValue: {
			create: jest.fn(),
		},
	},
]
```

Provider sind Abhängigkeiten, die NestJS über Dependency Injection zur Verfügung stellt.

Der echte Controller hat:

```typescript
constructor(private readonly groupsService: GroupsService) {}
```

Mit diesem Provider wird festgelegt:

> Wenn der Controller `GroupsService` anfordert, verwende dieses Mock-Objekt.

`provide` ist der Injection Token. Hier ist die Klasse `GroupsService` der Token. `useValue` sagt, welches konkrete Objekt stattdessen verwendet wird.

### 5.5 `.compile()`

```typescript
await Test.createTestingModule({...}).compile();
```

`.compile()` baut das Testmodul tatsächlich auf:

- Controller werden instanziiert.
- Provider werden registriert.
- Abhängigkeiten werden aufgelöst.
- Der Dependency-Injection-Container wird vorbereitet.

Es wird kein normales Produktions-Bundle gebaut. Es wird die Testumgebung erstellt.

`compile()` ist asynchron, deshalb stehen `await` und `async` im Setup.

### 5.6 `module.get`

```typescript
controller = module.get(GroupsController);
service = module.get(GroupsService);
```

`module.get` holt eine registrierte Instanz aus dem NestJS-Testcontainer.

In diesem Beispiel gilt:

- `controller` ist der echte `GroupsController`.
- `service` ist das Mock-Objekt aus `useValue`.
- Der echte Controller verwendet intern dieses gemockte Service-Objekt.

## 6. Vollständiger Controller-Test

Ein typisches `groups.controller.spec.ts` sieht so aus:

```typescript
import { Test, TestingModule } from '@nestjs/testing';
import { GroupsController } from './groups.controller';
import { GroupsService } from './groups.service';

describe('GroupsController', () => {
	let controller: GroupsController;
	let service: jest.Mocked<GroupsService>;

	beforeEach(async () => {
		const module: TestingModule = await Test.createTestingModule({
			controllers: [GroupsController],
			providers: [
				{
					provide: GroupsService,
					useValue: {
						create: jest.fn(),
						get: jest.fn(),
						findAll: jest.fn(),
						update: jest.fn(),
						deleteGroup: jest.fn(),
					},
				},
			],
		}).compile();

		controller = module.get(GroupsController);
		service = module.get(GroupsService);
	});

	it('POST /groups/create calls service', async () => {
		const dto = { name: 'A', description: 'B' };
		const expected = { id: '1', ...dto };
		service.create.mockResolvedValue(expected);

		const result = await controller.create(dto);

		expect(result).toEqual(expected);
		expect(service.create).toHaveBeenCalledWith(dto);
	});
});
```

### Ablauf dieses Tests

Der echte Controller enthält sinngemäß:

```typescript
async create(dto: CreateGroupsDto) {
	return this.groupsService.create(dto);
}
```

Der Test läuft so:

```text
1. dto wird als Eingabe erstellt.
2. Der Mock-Service wird so programmiert, dass er expected zurückgibt.
3. Der echte Controller wird mit dto aufgerufen.
4. Der Controller ruft den Mock-Service mit dto auf.
5. Der Mock-Service gibt expected zurück.
6. Der Controller gibt expected zurück.
7. expect prüft Ergebnis und Argumente.
```

### `service.create.mockResolvedValue(expected)`

Diese Zeile programmiert das Verhalten der Mock-Funktion:

```typescript
service.create.mockResolvedValue(expected);
```

Sie bedeutet:

> Wenn `service.create()` aufgerufen wird, liefere ein erfolgreich aufgelöstes Promise mit `expected`.

Ohne diese Zeile würde der Mock standardmäßig `undefined` zurückgeben. Dann könnte der Test nicht sinnvoll prüfen, ob der Controller die Rückgabe korrekt weiterleitet.

### `expect(service.create).toHaveBeenCalledWith(dto)`

```typescript
expect(service.create).toHaveBeenCalledWith(dto);
```

Diese Assertion prüft, ob die gemockte Service-Funktion mit genau dem DTO aufgerufen wurde.

Der Controller soll im Testfall machen:

```typescript
this.groupsService.create(dto);
```

Wenn er stattdessen versehentlich Folgendes machen würde, würde der Test fehlschlagen:

```typescript
this.groupsService.create({ name: 'falsch' });
```

Der Test prüft also nicht nur die Rückgabe, sondern auch die korrekte Delegation.

## 7. Controller-Tests für weitere Endpoints

```typescript
it('GET /groups/:id calls service.get', async () => {
	const expected = { id: '1', name: 'Test', description: 'Desc' };
	service.get.mockResolvedValue(expected);

	const result = await controller.get('1');

	expect(result).toEqual(expected);
	expect(service.get).toHaveBeenCalledWith('1');
});
```

```typescript
it('GET /groups calls service.findAll', async () => {
	const expected = [{ id: '1', name: 'Test', description: 'Desc' }];
	service.findAll.mockResolvedValue(expected);

	const result = await controller.findAll();

	expect(result).toEqual(expected);
	expect(service.findAll).toHaveBeenCalledWith();
});
```

```typescript
it('PATCH /groups/:id calls service.update', async () => {
	const dto = { name: 'Updated' };
	const expected = { id: '1', ...dto };
	service.update.mockResolvedValue(expected);

	const result = await controller.update('1', dto);

	expect(result).toEqual(expected);
	expect(service.update).toHaveBeenCalledWith('1', dto);
});
```

Bei einem Request-Objekt:

```typescript
it('DELETE /groups/:id passes userId to service', async () => {
	const request = { userId: 'u1' };

	await controller.remove('g1', request as any);

	expect(service.deleteGroup).toHaveBeenCalledWith('g1', 'u1', false);
});
```

## 8. Service-Tests

Der echte `GroupsService` hat Dependencies. Aus dem Constructor:

```typescript
constructor(
	@InjectRepository(Group)
	private readonly groupsRepository: Repository<Group>,
	@InjectRepository(UserGroup)
	private readonly userGroupsRepository: Repository<UserGroup>,
	private readonly logger: Logger,
) {}
```

Damit der Service im Test gebaut werden kann, müssen alle drei Dependencies registriert werden.

```typescript
providers: [
	GroupsService,
	provideMockRepository(Group),
	provideMockRepository(UserGroup),
	{
		provide: Logger,
		useValue: {
			log: jest.fn(),
			warn: jest.fn(),
			error: jest.fn(),
		},
	},
]
```

Der Service ist echt, die Repositories und der Logger sind Mocks.

### Service-Test für `create()`

```typescript
it('create() should create a group and return it', async () => {
	const dto = { name: 'Test', description: 'Desc' };
	const savedGroup = createMockGroup({ id: 'uuid-1', ...dto });

	repo.findOneBy.mockResolvedValue(null);
	repo.create.mockReturnValue(savedGroup);
	repo.save.mockResolvedValue(savedGroup);

	const result = await service.create(dto);

	expect(result).toEqual(new GroupsResponseDto(savedGroup));
	expect(repo.findOneBy).toHaveBeenCalledWith({ name: 'Test' });
	expect(repo.create).toHaveBeenCalledWith({
		name: 'Test',
		description: 'Desc',
	});
	expect(repo.save).toHaveBeenCalledWith(savedGroup);
});
```

Hier wird die echte Service-Methode ausgeführt. Die Mocks simulieren die Datenbank:

```text
findOneBy -> null       Name ist noch nicht vergeben
create    -> savedGroup Entity erstellen
save      -> savedGroup speichern
```

### Fehlerfall testen

```typescript
it('create() should throw if name already exists', async () => {
	repo.findOneBy.mockResolvedValue(
		createMockGroup({ id: 'existing', name: 'Test' }),
	);

	await expect(
		service.create({ name: 'Test', description: '' }),
	).rejects.toThrow(NotFoundException);

	expect(repo.create).not.toHaveBeenCalled();
	expect(repo.save).not.toHaveBeenCalled();
});
```

`.rejects` wird verwendet, weil die getestete Funktion asynchron fehlschlägt. Der Test prüft zusätzlich, dass nach dem Fehler keine neue Gruppe erstellt oder gespeichert wurde.

## 9. Test Utilities

### 9.1 Group Factory

```typescript
import { Group } from '../entities/group.entity';

export const createMockGroup = (
	overrides: Partial<Group> = {},
): Group => ({
	id: 'uuid-1',
	name: 'Test-Group',
	description: 'Test Description',
	isSystem: false,
	groupUsers: [],
	...overrides,
} as Group);
```

Die Factory erstellt Testdaten mit Standardwerten. `Partial<Group>` bedeutet, dass `overrides` nur einige Eigenschaften enthalten muss.

```typescript
const group = createMockGroup();

const systemGroup = createMockGroup({
	id: 'system-1',
	isSystem: true,
});
```

`groupUsers: []` ist nötig, weil diese Eigenschaft in der `Group`-Entity erforderlich ist:

```typescript
@OneToMany(() => UserGroup, ug => ug.group)
groupUsers!: UserGroup[];
```

### 9.2 Mock Repository

```typescript
import { getRepositoryToken } from '@nestjs/typeorm';
import { ObjectLiteral, Repository } from 'typeorm';

export const createMockRepository = <T extends ObjectLiteral>(
	entity: new () => T,
): jest.Mocked<Repository<T>> => ({
	findOneBy: jest.fn(),
	find: jest.fn(),
	create: jest.fn(),
	save: jest.fn(),
	update: jest.fn(),
	remove: jest.fn(),
	delete: jest.fn(),
} as any);

export const provideMockRepository = <T extends ObjectLiteral>(
	entity: new () => T,
) => ({
	provide: getRepositoryToken(entity),
	useValue: createMockRepository(entity),
});
```

`getRepositoryToken(Group)` erzeugt den Token, den NestJS für `Repository<Group>` verwendet. Dadurch kann der Test genau dieses Repository durch den Mock ersetzen.

## 10. Controller vs. Service direkt verglichen

| Controller-Test | Service-Test |
|---|---|
| Echter Controller | Echter Service |
| Service gemockt | Repositories gemockt |
| Prüft Parameterweitergabe | Prüft Business-Logik |
| Prüft Rückgabeweitergabe | Prüft Datenbankaufrufe |
| Wenige Abhängigkeiten | Alle Service-Abhängigkeiten nötig |
| Beispiel: `service.create(dto)` | Beispiel: `repo.save(entity)` |

Controller-Test:

```text
Test -> echter Controller -> Mock-Service -> expected
```

Service-Test:

```text
Test -> echter Service -> Mock-Repository -> simulierte DB-Antwort
```

## 11. Arrange, Act, Assert

Viele Tests folgen dem AAA-Muster:

### Arrange

Testdaten und Mock-Verhalten vorbereiten.

```typescript
const dto = { name: 'A', description: 'B' };
const expected = { id: '1', ...dto };
service.create.mockResolvedValue(expected);
```

### Act

Die zu testende Methode aufrufen.

```typescript
const result = await controller.create(dto);
```

### Assert

Ergebnis und Interaktionen prüfen.

```typescript
expect(result).toEqual(expected);
expect(service.create).toHaveBeenCalledWith(dto);
```

## 12. Häufige Fehler

### Dependency Injection Fehler

```text
Nest can't resolve dependencies of the GroupsService
```

Ursache: Eine Constructor-Dependency fehlt in `providers`.

Lösung: Für jede Dependency einen Provider oder Mock registrieren:

```typescript
GroupsService,
provideMockRepository(Group),
provideMockRepository(UserGroup),
{ provide: Logger, useValue: mockLogger },
```

### Logger Token

Wenn der Service importiert:

```typescript
import { Logger } from 'nestjs-pino';
```

dann sollte der Test normalerweise denselben Token verwenden:

```typescript
{ provide: Logger, useValue: mockLogger }
```

Ein String-Token wie `'Logger'` ist nicht automatisch dasselbe wie die Klasse `Logger`.

### `ObjectLiteral` Fehler

```text
Type 'T' does not satisfy the constraint 'ObjectLiteral'
```

Lösung:

```typescript
<T extends ObjectLiteral>
```

bei der generischen Funktion ergänzen.

### `mockReturnValue` oder `mockResolvedValue`

```typescript
repo.create.mockReturnValue(entity);        // synchron
repo.save.mockResolvedValue(entity);        // Promise
repo.findOneBy.mockResolvedValue(entity);   // Promise
```

### `groupUsers` fehlt

Wenn ein `Group`-Objekt für einen DTO-Constructor verwendet wird, muss es alle erforderlichen Entity-Eigenschaften enthalten. Dafür ist die Factory hilfreich:

```typescript
groupUsers: []
```

## 13. Ausführen der Tests

Im NestJS-Projekt:

```bash
npm test
```

Eine einzelne Testdatei:

```bash
npm test -- groups.service.spec.ts
npm test -- groups.controller.spec.ts
```

Watch-Modus:

```bash
npm run test:watch -- groups.service.spec.ts
```

Coverage:

```bash
npm run test:cov
```

Der Test-Output zeigt:

- `PASS`: Test-Suite erfolgreich.
- `FAIL`: Mindestens ein Test oder das Test-Setup fehlgeschlagen.
- `Test Suites`: Anzahl der Testdateien.
- `Tests`: Anzahl der einzelnen Tests.
- Fehlermeldungen zeigen meistens die fehlende Dependency, die falsche Assertion oder die Zeile im Test.

## 14. Leitfaden für neue Tests

### Einen Controller testen

1. Controller und Service importieren.
2. `describe` für den Controller anlegen.
3. Den echten Controller unter `controllers` registrieren.
4. Den Service unter `providers` als `useValue` mocken.
5. Jede verwendete Service-Methode mit `jest.fn()` anlegen.
6. Mit `module.get` Controller und Mock-Service holen.
7. Im Test Eingaben erstellen.
8. Mit `mockResolvedValue` die erwartete Service-Antwort definieren.
9. Die echte Controller-Methode aufrufen.
10. Rückgabewert und Service-Aufruf prüfen.

### Einen Service testen

1. Service und Entity-Typen importieren.
2. Alle Constructor-Dependencies identifizieren.
3. Für jedes Repository einen Repository-Mock registrieren.
4. Logger oder andere Provider mocken.
5. Den echten Service mit `module.get` holen.
6. Im Test Repository-Antworten vorbereiten.
7. Die echte Service-Methode aufrufen.
8. Ergebnis, Exceptions und Repository-Aufrufe prüfen.

### Welche Fälle testen?

Für jede Methode sind mindestens sinnvoll:

- Happy Path: Der normale erfolgreiche Fall.
- Not Found: Eine benötigte Entity existiert nicht.
- Konflikt: Zum Beispiel ein Gruppenname ist bereits vergeben.
- Forbidden: Ein User hat keine Berechtigung.
- Leere Ergebnisse: Zum Beispiel keine Gruppen oder Mitglieder.
- Keine unerwünschten Aufrufe nach einem Fehler.

## 15. Wichtigste Merksätze

> `describe` gruppiert Tests.

> `it` beschreibt einen einzelnen Test.

> `beforeEach` bereitet jeden Test neu vor.

> `jest.fn()` erstellt eine beobachtbare Mock-Funktion.

> `mockResolvedValue` simuliert eine erfolgreiche Promise-Antwort.

> `toHaveBeenCalledWith` prüft die Argumente eines Mock-Aufrufs.

> `Test.createTestingModule` baut einen isolierten NestJS-Testcontainer.

> `controllers` enthalten die echten Controller, die getestet werden.

> `providers` liefern Dependencies, häufig als Mocks.

> Controller-Tests testen Weiterleitung und Response.

> Service-Tests testen Business-Logik und Datenbankinteraktion.

> Arrange -> Act -> Assert ist ein guter Standardaufbau für Tests.
