# TypeORM - Zusammenfassung und wichtigste Funktionen

TypeORM ist ein ORM (Object-Relational Mapper) für TypeScript und JavaScript. Es verbindet Klassen und Objekte im Code mit Tabellen und Zeilen in einer relationalen Datenbank wie PostgreSQL.

## 1. Grundidee

Ohne ORM würde man SQL direkt schreiben:

```sql
SELECT * FROM groups WHERE id = 'group-1';
```

Mit TypeORM arbeitet man mit TypeScript-Klassen und Repositories:

```typescript
const group = await groupsRepository.findOneBy({ id: 'group-1' });
```

TypeORM übersetzt den Repository-Aufruf intern in SQL.

### Begriffe

| Begriff | Bedeutung |
|---|---|
| Entity | TypeScript-Klasse, die einer Datenbanktabelle entspricht |
| Property | Eigenschaft einer Entity, entspricht meist einer Tabellenspalte |
| Repository | Objekt für Datenbankoperationen einer Entity |
| Relation | Verbindung zwischen Entities beziehungsweise Tabellen |
| Primary Key | Eindeutige ID einer Zeile |
| Migration | Versionierte Änderung des Datenbankschemas |
| Query Builder | API zum Erstellen komplexerer SQL-Abfragen |

## 2. Der typische Datenfluss

```text
Controller
		|
		v
Service
		|
		v
TypeORM Repository
		|
		v
Datenbank
```

Beispiel:

```typescript
@Injectable()
export class GroupsService {
	constructor(
		@InjectRepository(Group)
		private readonly groupsRepository: Repository<Group>,
	) {}

	async get(id: string): Promise<Group | null> {
		return this.groupsRepository.findOneBy({ id });
	}
}
```

Der Service kennt das Repository, aber nicht die konkrete SQL-Syntax.

## 3. Eine Entity definieren: `@Entity()`

### Was macht der Decorator?

`@Entity()` markiert eine Klasse als Datenbank-Entity. TypeORM behandelt sie als Tabelle.

```typescript
@Entity('groups')
export class Group {
	// Spalten folgen hier
}
```

- `'groups'` ist der Tabellenname.
- Ohne Namen würde TypeORM einen Namen aus dem Klassennamen ableiten.

## 4. Spalten-Decorators

### 4.1 `@PrimaryGeneratedColumn()`

Erstellt den Primary Key. Mit `'uuid'` erzeugt TypeORM UUIDs.

```typescript
@PrimaryGeneratedColumn('uuid')
id!: string;
```

- Eingabe: optional der ID-Typ, hier `'uuid'`.
- Laufzeitwert: TypeORM erzeugt die ID beim Speichern.
- `!` ist nur eine TypeScript-Non-Null-Assertion; TypeORM setzt die ID später.

### 4.2 `@Column()`

Definiert eine normale Tabellenspalte.

```typescript
@Column({ type: 'varchar', unique: true })
name!: string;
```

Wichtige Optionen:

| Option | Bedeutung |
|---|---|
| `type` | Datenbanktyp |
| `unique` | Wert darf nicht doppelt vorkommen |
| `nullable` | `NULL` ist erlaubt |
| `default` | Standardwert beim Einfügen |
| `name` | Datenbank-Spaltenname |

```typescript
@Column({ type: 'varchar', nullable: true })
description!: string | null;

@Column({ type: 'boolean', name: 'is_system', default: false })
isSystem!: boolean;
```

`name: 'is_system'` bedeutet: Im TypeScript heißt die Property `isSystem`, in PostgreSQL heißt die Spalte `is_system`.

### 4.3 `@CreateDateColumn()` und `@UpdateDateColumn()`

Diese Decorators verwalten Zeitstempel automatisch.

```typescript
@CreateDateColumn({ type: 'timestamptz', name: 'created_at' })
createdAt!: Date;

@UpdateDateColumn({ type: 'timestamptz', name: 'updated_at' })
updatedAt!: Date;
```

- `@CreateDateColumn()` wird beim Erstellen gesetzt.
- `@UpdateDateColumn()` wird beim Aktualisieren angepasst.

## 5. Vollständige `Group`-Entity

```typescript
@Entity('groups')
export class Group {
	@PrimaryGeneratedColumn('uuid')
	id!: string;

	@Column({ type: 'varchar', unique: true })
	name!: string;

	@Column({ type: 'varchar', nullable: true })
	description!: string | null;

	@Column({ type: 'boolean', name: 'is_system', default: false })
	isSystem!: boolean;

	@OneToMany(() => UserGroup, userGroup => userGroup.group)
	groupUsers!: UserGroup[];
}
```

Die Klasse beschreibt die Tabelle `groups`. Jede Instanz repräsentiert normalerweise eine Zeile.

## 6. Repository in NestJS verwenden

### `TypeOrmModule.forFeature()`

Damit NestJS ein Repository in ein Modul injizieren kann, muss die Entity im Modul registriert werden:

```typescript
@Module({
	imports: [TypeOrmModule.forFeature([Group])],
})
export class GroupsModule {}
```

### `@InjectRepository()`

Der Decorator injiziert das Repository in den Constructor:

```typescript
constructor(
	@InjectRepository(Group)
	private readonly groupsRepository: Repository<Group>,
) {}
```

- `Group` bestimmt, für welche Tabelle das Repository arbeitet.
- `Repository<Group>` gibt den TypeScript-Typ an.
- `groupsRepository` ist danach für Datenbankoperationen verfügbar.

## 7. `repository.create()`

### Was macht die Methode?

`create()` erstellt eine Entity-Instanz im Speicher. Sie speichert noch nichts in der Datenbank.

```typescript
const group = groupsRepository.create({
	name: 'Chess Club',
	description: 'A group for chess players',
});
```

- Eingabe: ein Objekt mit Entity-Eigenschaften.
- Rückgabe: eine `Group`-Entity im Speicher.
- Datenbank: noch keine Änderung.

### Wann verwendet man sie?

Vor `save()`, wenn eine neue Entity angelegt werden soll.

```typescript
const group = this.groupsRepository.create(dto);
await this.groupsRepository.save(group);
```

In Tests ist `create()` normalerweise synchron:

```typescript
repo.create.mockReturnValue(group);
```

## 8. `repository.save()`

### Was macht die Methode?

`save()` speichert eine Entity. Bei einer neuen Entity wird normalerweise ein `INSERT` ausgeführt. Bei einer Entity mit vorhandener ID kann ein `UPDATE` ausgeführt werden.

```typescript
const group = groupsRepository.create({ name: 'Chess Club' });
const savedGroup = await groupsRepository.save(group);
```

- Eingabe: eine Entity oder ein Array von Entities.
- Rückgabe: ein Promise mit der gespeicherten Entity beziehungsweise dem Array.
- Die Datenbank wird verändert.

```typescript
await this.groupsRepository.save(group);
```

In Tests:

```typescript
repo.save.mockResolvedValue(savedGroup);
```

## 9. `repository.findOneBy()`

### Was macht die Methode?

Sucht eine einzelne Entity anhand einfacher Bedingungen.

```typescript
const group = await groupsRepository.findOneBy({ id: groupId });
```

- Eingabe: ein `where`-Objekt.
- Rückgabe: `Promise<Group | null>`.
- Wenn nichts gefunden wird, kommt `null` zurück.

Weitere Beispiele:

```typescript
const group = await groupsRepository.findOneBy({ name: 'Chess Club' });
const activeGroup = await groupsRepository.findOneBy({ isSystem: false });
```

Typischer Service-Code:

```typescript
const group = await this.groupsRepository.findOneBy({ id });
if (!group) {
	throw new NotFoundException('Group not found');
}
```

## 10. `repository.findOne()`

### Was macht die Methode?

`findOne()` sucht ebenfalls eine einzelne Entity, bietet aber mehr Optionen, zum Beispiel Relations.

```typescript
const group = await groupsRepository.findOne({
	where: { id: groupId },
	relations: { groupUsers: true },
});
```

- Eingabe: ein vollständiges `FindOneOptions`-Objekt.
- Rückgabe: `Promise<Group | null>`.
- Verwendung: wenn neben `where` noch `relations`, `select` oder `order` benötigt werden.

Faustregel:

```typescript
findOneBy({ id });                 // einfache Suche
findOne({ where: { id }, ... });   // Suche mit Optionen
```

## 11. `repository.find()`

### Was macht die Methode?

`find()` lädt mehrere Entities.

```typescript
const groups = await groupsRepository.find();
```

- Eingabe: optional Such- und Ladeoptionen.
- Rückgabe: `Promise<Group[]>`.
- Wenn nichts gefunden wird, kommt ein leeres Array zurück, nicht `null`.

Mit Filter:

```typescript
const groups = await this.groupsRepository.find({
	where: { isSystem: false },
});
```

Mit Relations und Sortierung:

```typescript
const memberships = await userGroupRepository.find({
	where: { userId },
	relations: { group: true },
	order: { joinedAt: 'DESC' },
});
```

## 12. `repository.findAndCount()`

### Was macht die Methode?

`findAndCount()` lädt Entities und liefert gleichzeitig die Gesamtanzahl aller passenden Datensätze.

```typescript
const [groups, total] = await groupsRepository.findAndCount({
	skip: 0,
	take: 20,
});
```

- Eingabe: Such-, Sortier- und Pagination-Optionen.
- Rückgabe: `[Group[], number]`.
- `groups` enthält die aktuelle Seite.
- `total` enthält die Gesamtzahl ohne Pagination.

Pagination-Beispiel:

```typescript
const page = 2;
const limit = 20;

const [groups, total] = await groupsRepository.findAndCount({
	skip: (page - 1) * limit,
	take: limit,
	order: { name: 'ASC' },
});

const totalPages = Math.ceil(total / limit);
```

## 13. `repository.update()`

### Was macht die Methode?

`update()` aktualisiert Spalten direkt anhand eines Kriteriums.

```typescript
const result = await groupsRepository.update(
	groupId,
	{ name: 'New name' },
);
```

- Eingabe: Kriterium oder ID und die zu ändernden Werte.
- Rückgabe: ein Promise mit `UpdateResult`.
- `result.affected` zeigt, wie viele Zeilen betroffen waren.

```typescript
if (result.affected === 0) {
	throw new NotFoundException('Group not found');
}
```

`update()` gibt normalerweise nicht die vollständig aktualisierte Entity zurück. Wenn du sie brauchst, musst du sie danach erneut laden:

```typescript
await this.groupsRepository.update(id, dto);
const updatedGroup = await this.groupsRepository.findOneBy({ id });
```

## 14. `repository.remove()`

### Was macht die Methode?

`remove()` löscht eine geladene Entity aus der Datenbank.

```typescript
const group = await groupsRepository.findOneBy({ id });

if (group) {
	await groupsRepository.remove(group);
}
```

- Eingabe: eine geladene Entity oder ein Array.
- Rückgabe: ein Promise mit der entfernten Entity beziehungsweise dem Array.
- Die Entity sollte normalerweise vorher geladen werden.

Typischer Ablauf:

```typescript
const group = await this.groupsRepository.findOneBy({ id });
if (!group) {
	throw new NotFoundException('Group not found');
}

await this.groupsRepository.remove(group);
```

## 15. `repository.delete()`

### Was macht die Methode?

`delete()` löscht direkt anhand eines Kriteriums, ohne die Entity vorher zu laden.

```typescript
const result = await userGroupRepository.delete({
	userId,
	groupId,
});
```

- Eingabe: ID oder Kriterium.
- Rückgabe: ein Promise mit `DeleteResult`.
- `result.affected` zeigt, ob eine Zeile gelöscht wurde.

```typescript
if (!result.affected) {
	throw new NotFoundException('Membership not found');
}
```

### `remove()` oder `delete()`?

| Methode | Verhalten |
|---|---|
| `remove(entity)` | Entity ist geladen; Entity-Lifecycle und Relations können berücksichtigt werden |
| `delete(criteria)` | Direkter Delete anhand von ID oder Kriterien |

Für die User-Group-Zuordnung ist `delete({ userId, groupId })` oft praktisch, weil die zusammengesetzte Zuordnung direkt bekannt ist.

## 16. `repository.preload()`

### Was macht die Methode?

`preload()` lädt eine vorhandene Entity und verbindet sie mit neuen Werten. Das Ergebnis ist eine aktualisierte Entity im Speicher, die anschließend gespeichert werden kann.

```typescript
const group = await groupsRepository.preload({
	id: groupId,
	name: 'Updated name',
});

if (!group) {
	throw new NotFoundException('Group not found');
}

await groupsRepository.save(group);
```

- Eingabe: Objekt mit einer vorhandenen ID und neuen Werten.
- Rückgabe: `Promise<Group | undefined>`.
- Datenbankänderung: erst durch `save()`.

`preload()` ist nützlich, wenn du eine vollständige Entity vor dem Speichern brauchst.

## 17. Suchbedingungen mit `where`

Einfache Bedingungen werden als Objekt angegeben:

```typescript
const groups = await groupsRepository.find({
	where: {
		isSystem: false,
	},
});
```

Mehrere Properties bedeuten normalerweise `AND`:

```typescript
const group = await groupsRepository.findOneBy({
	id: groupId,
	isSystem: false,
});
```

Mehrere Bedingungen als Array können `OR` ausdrücken:

```typescript
const groups = await groupsRepository.find({
	where: [
		{ name: 'Chess Club' },
		{ name: 'Tennis Club' },
	],
});
```

## 18. TypeORM-Werte und Operatoren

Für komplexere Bedingungen gibt es TypeORM-Operatoren:

```typescript
import { ILike, MoreThan, In, IsNull } from 'typeorm';

const groups = await groupsRepository.find({
	where: {
		name: ILike('%club%'),
	},
});
```

Häufige Operatoren:

| Operator | Beispiel | Bedeutung |
|---|---|---|
| `Equal(value)` | `Equal('Chess')` | exakt gleich |
| `Not(value)` | `Not('Admin')` | nicht gleich |
| `In(values)` | `In(['a', 'b'])` | Wert ist in Liste |
| `MoreThan(value)` | `MoreThan(10)` | größer als |
| `LessThan(value)` | `LessThan(10)` | kleiner als |
| `Between(a, b)` | `Between(1, 10)` | zwischen zwei Werten |
| `ILike(pattern)` | `ILike('%club%')` | PostgreSQL-Suche ohne Beachtung der Großschreibung |
| `IsNull()` | `IsNull()` | ist `NULL` |

```typescript
const groups = await groupsRepository.find({
	where: {
		id: In(groupIds),
	},
});
```

## 19. Relations zwischen Entities

Relations beschreiben Verbindungen zwischen Tabellen.

### 19.1 `@ManyToOne()`

Viele Datensätze gehören zu einer anderen Entity.

```typescript
@ManyToOne(() => Group, { onDelete: 'CASCADE' })
@JoinColumn({ name: 'group_id' })
group!: Group;
```

Bedeutung: Viele `UserGroup`-Einträge können auf eine `Group` zeigen.

### 19.2 `@OneToMany()`

Eine Entity besitzt viele verbundene Datensätze.

```typescript
@OneToMany(() => UserGroup, userGroup => userGroup.group)
groupUsers!: UserGroup[];
```

Bedeutung: Eine `Group` kann viele `UserGroup`-Einträge besitzen.

`@OneToMany()` ist normalerweise die inverse Seite. Die eigentliche Foreign-Key-Spalte liegt bei `@ManyToOne()`.

### 19.3 `@OneToOne()` und `@ManyToMany()`

```typescript
@OneToOne(() => Profile)
profile!: Profile;
```

Eine Entity ist mit genau einer anderen Entity verbunden.

```typescript
@ManyToMany(() => Permission)
permissions!: Permission[];
```

Viele Entities können mit vielen anderen Entities verbunden sein. Dafür wird meist eine Join-Tabelle benötigt.

## 20. `@JoinColumn()`

### Was macht der Decorator?

`@JoinColumn()` legt fest, welche Spalte den Foreign Key enthält.

```typescript
@ManyToOne(() => Group)
@JoinColumn({ name: 'group_id' })
group!: Group;
```

- TypeScript-Property: `group`.
- Datenbankspalte: `group_id`.
- Foreign Key: verweist auf die `groups`-Tabelle.

Bei `@ManyToMany()` verwendet man typischerweise `@JoinTable()` auf einer Seite:

```typescript
@ManyToMany(() => Permission)
@JoinTable()
permissions!: Permission[];
```

## 21. Relations laden

Relations werden nicht immer automatisch mitgeladen. Man kann sie explizit anfordern:

```typescript
const memberships = await userGroupRepository.find({
	where: { userId },
	relations: { group: true },
});

for (const membership of memberships) {
	console.log(membership.group.name);
}
```

Alternativ mit `findOne()`:

```typescript
const group = await groupsRepository.findOne({
	where: { id: groupId },
	relations: { groupUsers: true },
});
```

Ohne `relations` kann `membership.group` `undefined` sein, obwohl die Relation in der Entity definiert ist.

## 22. `onDelete`

`onDelete` beschreibt, was mit abhängigen Datensätzen passiert, wenn der referenzierte Datensatz gelöscht wird.

```typescript
@ManyToOne(() => Group, { onDelete: 'CASCADE' })
group!: Group;
```

| Wert | Verhalten |
|---|---|
| `CASCADE` | abhängige Zeilen werden mitgelöscht |
| `SET NULL` | Foreign Key wird auf `NULL` gesetzt |
| `RESTRICT` | Löschen wird verhindert, wenn abhängige Zeilen existieren |

`SET NULL` erfordert, dass die Foreign-Key-Spalte nullable ist.

## 23. Composite Primary Keys

Eine Relationstabelle wie `user_groups` kann aus zwei Spalten bestehen, die zusammen eindeutig sind:

```typescript
@Entity('user_groups')
export class UserGroup {
	@PrimaryColumn({ type: 'uuid', name: 'user_id' })
	userId!: string;

	@PrimaryColumn({ type: 'uuid', name: 'group_id' })
	groupId!: string;
}
```

Die Kombination aus `userId` und `groupId` ist der Primary Key. Derselbe User kann dadurch nicht zweimal derselben Gruppe zugeordnet werden.

Suche und Delete verwenden beide Teile:

```typescript
const membership = await userGroupRepository.findOneBy({
	userId,
	groupId,
});

await userGroupRepository.delete({ userId, groupId });
```

## 24. `Repository<T>` und Generics

```typescript
Repository<Group>
Repository<UserGroup>
```

`T` ist ein Generic und steht für die Entity, mit der das Repository arbeitet.

```typescript
const groupsRepository: Repository<Group> = ...;
```

TypeScript weiß dadurch zum Beispiel, dass `findOneBy()` eine `Group` oder `null` zurückgibt.

In einer Hilfsfunktion muss der Generic-Typ die TypeORM-Anforderung erfüllen:

```typescript
import { ObjectLiteral, Repository } from 'typeorm';

function useRepository<T extends ObjectLiteral>(
	repository: Repository<T>,
) {
	return repository;
}
```

`T extends ObjectLiteral` bedeutet, dass `T` ein passender Objekt-Typ sein muss.

## 25. Transaktionen

Eine Transaktion fasst mehrere Datenbankoperationen zusammen. Entweder werden alle erfolgreich ausgeführt oder bei einem Fehler zurückgerollt.

```typescript
await dataSource.transaction(async manager => {
	const group = manager.create(Group, {
		name: 'Chess Club',
	});

	await manager.save(group);
	await manager.insert(UserGroup, {
		userId: 'user-1',
		groupId: group.id,
	});
});
```

Innerhalb der Transaktion sollte man den übergebenen `manager` verwenden und nicht versehentlich das normale Repository außerhalb der Transaktion.

## 26. Query Builder

Für komplexe Abfragen kann man den Query Builder verwenden.

```typescript
const groups = await groupsRepository
	.createQueryBuilder('group')
	.where('group.is_system = :isSystem', { isSystem: false })
	.andWhere('group.name ILIKE :name', { name: '%club%' })
	.orderBy('group.name', 'ASC')
	.getMany();
```

### Wichtige Query-Builder-Methoden

| Methode | Bedeutung |
|---|---|
| `createQueryBuilder()` | Query Builder starten |
| `where()` | erste Bedingung setzen |
| `andWhere()` | `AND`-Bedingung hinzufügen |
| `orWhere()` | `OR`-Bedingung hinzufügen |
| `leftJoinAndSelect()` | Relation verbinden und laden |
| `orderBy()` | sortieren |
| `skip()` | Datensätze überspringen |
| `take()` | Anzahl begrenzen |
| `getOne()` | eine Entity laden |
| `getMany()` | mehrere Entities laden |
| `getCount()` | Anzahl laden |
| `getRawMany()` | rohe Daten statt Entities laden |

Parameter sollten immer über Platzhalter übergeben werden:

```typescript
.where('group.name = :name', { name });
```

Das ist sicherer als Werte direkt in SQL-Strings einzusetzen.

## 27. `Repository.insert()` und `Repository.upsert()`

### `insert()`

Fügt Daten direkt ein, ohne vorher eine Entity-Instanz zu erstellen.

```typescript
await groupsRepository.insert({
	name: 'Chess Club',
	description: 'Players welcome',
});
```

### `upsert()`

Fügt einen Datensatz ein oder aktualisiert ihn, wenn ein Konflikt mit einem eindeutigen Schlüssel besteht.

```typescript
await groupsRepository.upsert(
	{ id: groupId, name: 'Chess Club' },
	['id'],
);
```

Für normale Business-Logik sind `create()` und `save()` oft verständlicher. `insert()` und `upsert()` sind nützlich bei direkten oder vielen Datenbankoperationen.

## 28. Migrationen

Eine Migration beschreibt eine kontrollierte Änderung am Datenbankschema.

```typescript
export class AddDescriptionToGroups1710000000000 {
	async up(queryRunner: QueryRunner): Promise<void> {
		await queryRunner.addColumn(
			'groups',
			new TableColumn({
				name: 'description',
				type: 'varchar',
				isNullable: true,
			}),
		);
	}

	async down(queryRunner: QueryRunner): Promise<void> {
		await queryRunner.dropColumn('groups', 'description');
	}
}
```

- `up()` wendet die Änderung an.
- `down()` macht die Änderung rückgängig.

Migrationen sind besser für gemeinsame Projekte und Production als automatische Schemaänderungen durch `synchronize`.

## 29. `synchronize`

In der TypeORM-Konfiguration kann `synchronize` Entities automatisch in das Datenbankschema übertragen:

```typescript
TypeOrmModule.forRoot({
	type: 'postgres',
	entities: [Group, UserGroup],
	synchronize: false,
});
```

- `true`: bequem für lokale Experimente.
- `false`: sicherer für Production; Änderungen über Migrationen.

`synchronize: true` sollte man in Production vermeiden, weil automatische Schemaänderungen Datenverlust verursachen können.

## 30. TypeORM im Unit-Test mocken

In einem Unit-Test wird normalerweise keine echte Datenbank verbunden. Stattdessen wird das Repository gemockt:

```typescript
const repositoryMock = {
	findOneBy: jest.fn(),
	find: jest.fn(),
	create: jest.fn(),
	save: jest.fn(),
	update: jest.fn(),
	remove: jest.fn(),
	delete: jest.fn(),
};
```

In NestJS:

```typescript
{
	provide: getRepositoryToken(Group),
	useValue: repositoryMock,
}
```

Beispiel für ein Verhalten:

```typescript
repo.findOneBy.mockResolvedValue(null);
repo.create.mockReturnValue(group);
repo.save.mockResolvedValue(group);
```

Das simuliert:

```text
Kein Gruppenname ist belegt.
Eine Group-Entity wird erstellt.
Die Entity wird erfolgreich gespeichert.
```

## 31. Wichtigste Repository-Funktionen als Tabelle

| Funktion | Eingabe | Rückgabe | Typischer Zweck |
|---|---|---|---|
| `create()` | Entity-Daten | Entity im Speicher | neue Entity vorbereiten |
| `save()` | Entity oder Array | Promise mit gespeicherter Entity | Insert oder Update |
| `findOneBy()` | einfache Bedingungen | Entity oder `null` | eine Entity suchen |
| `findOne()` | Optionen mit `where`, Relations usw. | Entity oder `null` | eine Entity mit Optionen suchen |
| `find()` | optionale Suchoptionen | Array | mehrere Entities laden |
| `findAndCount()` | Such- und Pagination-Optionen | `[Array, Anzahl]` | Pagination |
| `update()` | ID/Kriterium und Werte | `UpdateResult` | direkt aktualisieren |
| `remove()` | geladene Entity | entfernte Entity | Entity löschen |
| `delete()` | ID/Kriterium | `DeleteResult` | direkt löschen |
| `preload()` | ID und neue Werte | Entity oder `undefined` | Entity laden und vorbereiten |
| `insert()` | Daten | `InsertResult` | direkt einfügen |
| `upsert()` | Daten und Konfliktspalten | `InsertResult` | einfügen oder aktualisieren |
| `createQueryBuilder()` | Alias | Query Builder | komplexe Abfragen |

## 32. Mentaler Leitfaden

### Neue Entity anlegen

```text
1. Entity-Klasse mit @Entity() definieren.
2. Spalten mit @Column() definieren.
3. Primary Key definieren.
4. Relations definieren, falls nötig.
5. Entity mit forFeature() im Nest-Modul registrieren.
6. Repository mit @InjectRepository() injizieren.
7. Daten mit create() vorbereiten.
8. Daten mit save() speichern.
```

### Entity laden

```text
1. Entscheiden: eine Entity oder mehrere?
2. Einfache Suche: findOneBy().
3. Suche mit Relations/Optionen: findOne().
4. Mehrere Ergebnisse: find().
5. Ergebnis auf null oder leeres Array prüfen.
6. Falls nötig DTO statt Entity zurückgeben.
```

### Entity ändern

```text
1. Entity laden.
2. Nicht gefunden: NotFoundException.
3. Fachliche Regeln prüfen.
4. update() oder preload() + save() verwenden.
5. Ergebnis gegebenenfalls erneut laden.
```

### Entity löschen

```text
1. Entity oder Relation finden.
2. Berechtigung prüfen.
3. remove() für geladene Entities oder delete() für Kriterien verwenden.
4. affected beziehungsweise Ergebnis prüfen.
```

## 33. Häufige Fehler

| Fehler | Ursache | Lösung |
|---|---|---|
| Repository kann nicht injiziert werden | Entity fehlt in `forFeature()` | `TypeOrmModule.forFeature([Entity])` ergänzen |
| `Nest can't resolve dependencies` | Provider oder Repository fehlt im Test | Dependency mocken und registrieren |
| Ergebnis ist `null` | `findOneBy()` findet nichts | `if (!entity)` prüfen |
| Ergebnis ist `[]` | `find()` findet nichts | Leeres Array behandeln |
| Relation ist `undefined` | Relation wurde nicht geladen | `relations: { relation: true }` verwenden |
| `update()` liefert keine Entity | `update()` gibt nur `UpdateResult` zurück | Entity danach erneut laden |
| `remove()` funktioniert nicht | Keine geladene Entity übergeben | Erst `findOneBy()` aufrufen |
| Datenbankspalte heißt falsch | Property- und Spaltenname unterscheiden sich | `@Column({ name: '...' })` prüfen |
| Migration fehlt | Entity und Datenbank sind nicht synchron | Migration erstellen und ausführen |
| Duplicate-Key-Fehler | `unique` oder Primary Key verletzt | Vorher prüfen oder Konflikt fachlich behandeln |

## 34. Wichtigste Merksätze

> Eine Entity beschreibt eine Tabelle.

> Eine Entity-Instanz repräsentiert Daten im Anwendungscode.

> Ein Repository führt Datenbankoperationen für eine Entity aus.

> `create()` erstellt nur im Speicher; `save()` schreibt in die Datenbank.

> `findOneBy()` gibt eine Entity oder `null` zurück.

> `find()` gibt immer ein Array zurück, bei keinen Treffern `[]`.

> `update()` gibt normalerweise kein aktualisiertes Entity-Objekt zurück.

> `remove()` löscht eine geladene Entity; `delete()` löscht direkt nach Kriterien.

> Relations müssen häufig ausdrücklich geladen werden.

> `@ManyToOne()` besitzt normalerweise den Foreign Key; `@OneToMany()` ist die inverse Seite.

> In Production sind Migrationen sicherer als `synchronize: true`.

> Im Unit-Test werden Repositories gemockt, damit keine echte Datenbank benötigt wird.
