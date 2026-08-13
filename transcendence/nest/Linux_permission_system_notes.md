# Linux Permission System 

## r,w,x directory

### r - lesen
  - entries listen
bspw fuer:
  - `ls directory/`

### w - directory veraendern
  - allowed to add/delete/rename entries
  - bedeutet nicht automatisch, dass man entry content changen darf
bspw fuer:
- `rm project/a.txt`
=> nur nicht `w` auf `a.txt`

### x - directory betreten / traversieren
  - allowed to modify objects within the directory - if filename is known and other permissions allow it
    - wenn `--x` dann `ls directory` not allowed aber `cat directory/secret.txt` schon  

## Role separation

- User sind immer nur einer Rolle (owner, group, Other) zugeschrieben - `owner` kann nicht memeber of `group` sein

## Multiple groups

- file/directory gehoert standardmaessig immer nur einer group
- Bob ist member of group `developers` & `managers` => file permissions `-rw-r----- alice developers` dann hat er auf den File nur die permissions, die developers zugewiesen sind...

## File creator != file owner

1. `touch test.txt`
2. `chown bob test.txt`/`chown bob:developers test.txt`

### changing file group

- `chgrp developers test.txt`

## `chmod` syntax

`chmod 755 file`
```zsh
chmod u+rwx file
chmod g+rx file
chmod o-r file

u = user/owner
g = group
o = others
a = all
```

## Execute file
- `-rwxr-xr-x move_home_to_goinfre.sh` allows `./move_home_to_goinfre.sh` 
- ohne `x` => `Permission denied`

#### ABER possibly
- `bash move_home_to_goinfre.sh` moeglich
 - => bash liest die Datei und fuehrt sie aus - brauchst nur `r` nicht `x`...

#### "Permissions beschreiben nicht "was die Datei ist", sondern welche Operation ein Prozess auf dem Objekt durchführen darf."

---

# Own permission system

1. For file:
```
canRead(file)
canWrite(file)
canDelete(file)

```
2. For directory:
```
canList(directory)
canTraverse(directory)
canCreate(directory)
canDeleteFrom(directory)
```