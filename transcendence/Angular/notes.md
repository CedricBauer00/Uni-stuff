# Angular notes

1. computed() function
Erstellt ein derived signal – ein Signal, das automatisch neu berechnet wird, wenn sich seine Abhängigkeiten ändern.

```
readonly isAdminUser = computed(() => this.currentUser()?.isAdmin === true);
```
- `currentUser()` ist ein Signal
- `isAdminUser` ist ein Signal, das true/false liefert

2. Was sind interfaces?

Bspw anfangs eines fiels (auth.ts)
```
export interface LoginRequest {
	email: string;
	password: string;
}

export interface LoginResponse {
	id: string;
	email: string;
	displayName: string;
	preferredLanguage: SupportedLanguage;
}

export interface CurrentUser {
	id: string;
	email: string;
	displayName: string;
	preferredLanguage: SupportedLanguage;
	isAdmin: boolean; 
}
```
Tabelle:
LoginRequest	| Was du schickst beim Login (email, password)
LoginResponse	| Was der Server beim Login zurückgibt (Token, User-Daten)
CurrentUser	  | Was der angemeldete User im Frontend hat (inkl. isAdmin)

- Trennung von Request/Response/State = saubere Typisierung, keine Vermischung.

3. `readonly`
- property initialization (Class field) - Feld-Deklaration mit Initialwert
- Wird einmal beim Instanziieren der Klasse ausgeführt
- readonly = kann nachträglich nicht überschrieben werden (wie `const` in c)

4. @Component Decorator – Was ist das?

Markiert die Klasse als Angular Component.
Angular erkennt: "Das ist eine UI-Komponente, die ich instanziieren, rendern und in den Komponenten-Baum einhängen kann."

Ohne @Component → nur eine normale TypeScript-Klasse, Angular kennt sie nicht.

Bsp: admin-groups.ts
```
@Component({
  selector: 'app-admin-groups',
  templateUrl: './admin-groups.html',
  styleUrl: './admin-groups.scss',
})
export class AdminGroups implements OnInit {}
```
4.1. styleUrl: './admin-groups.scss'
- Der HTML-Tag-Name, mit dem du die Komponente in anderen Templates einbindest.
```
<!-- In einem anderen Template (z. B. AdminShell oder Route) -->
<app-admin-groups></app-admin-groups>
```
- Muss eindeutig sein (Präfix app- ist Konvention)
- Wird beim Kompilieren zu einem Custom Element registriert

4.2. templateUrl: './admin-groups.html'
- Pfad zur HTML-Datei, die das Markup/Template der Komponente enthält.
- Wird zur Laufzeit geladen und gerendert
- Alternative: template: '<div>Inline HTML</div>' (inline, aber unübersichtlich bei größeren Templates)

4.3. styleUrl: './admin-groups.scss'
- Pfad zur SCSS/CSS-Datei für komponenten-lokale Styles.
- Styles sind gekapselt (Shadow DOM-ähnlich via Attribut-Selektoren)
- Gelten nur für diese Komponente, nicht global
- Alternative: styles: ['.foo { color: red }'] (inline)

4.4. implements OnInit
- Interface aus @angular/core – sagt: "Diese Komponente hat eine ngOnInit() Methode."
```
export class AdminGroups implements OnInit {
  ngOnInit() {
    // Wird **einmal** nach Erstellung der Komponente aufgerufen
    // Hier: Daten laden, Subscriptions starten, Initialisierung
  }
}
```
