import os

def demander_dossier_cible():
    while True:
        dossier = input("Entrez le chemin **relatif** du dossier à scanner (ex : ./Source) : ").strip()
        
        # Empêche les chemins absolus
        if os.path.isabs(dossier):
            print("❌ Le chemin doit être **relatif**, pas absolu. Exemples valides : ./Source ou Source/Characters\n")
            continue
        
        chemin_complet = os.path.abspath(dossier)
        if os.path.isdir(chemin_complet):
            return dossier  # On retourne le chemin relatif conservé tel quel
        else:
            print(f"❌ Dossier introuvable : '{dossier}'\nVeuillez réessayer.\n")

def demander_nom_fichier_sortie():
    nom_fichier = input("Entrez le nom du fichier de sortie (ex : output.txt) : ").strip()
    return nom_fichier if nom_fichier else "output.txt"

def concat_cpp_headers(dossier_relatif: str, nom_fichier_sortie: str):
    """
    Concatène les .cpp/.h d’un dossier donné (relatif) dans un fichier de sortie,
    avec des séparateurs indiquant le chemin relatif de chaque fichier.
    """
    dossier_complet = os.path.abspath(dossier_relatif)
    fichiers_trouves = []

    for dirpath, _, filenames in os.walk(dossier_complet):
        for filename in filenames:
            if filename.endswith(('.cpp', '.h')):
                full_path = os.path.join(dirpath, filename)
                fichiers_trouves.append(full_path)

    with open(nom_fichier_sortie, 'w', encoding='utf-8') as out:
        for chemin_absolu in fichiers_trouves:
            chemin_relatif_fichier = os.path.relpath(chemin_absolu, dossier_complet)
            out.write(f"\n############## {chemin_relatif_fichier} ###############\n")

            try:
                with open(chemin_absolu, 'r', encoding='utf-8') as f:
                    contenu = f.read()
            except UnicodeDecodeError:
                with open(chemin_absolu, 'r', encoding='latin-1') as f:
                    contenu = f.read()

            out.write(contenu)
            out.write("\n")

    print(f"\n✅ {len(fichiers_trouves)} fichiers concaténés.")
    print(f"📄 Résultat écrit dans : {os.path.abspath(nom_fichier_sortie)}")

if __name__ == "__main__":
    dossier = demander_dossier_cible()
    sortie = demander_nom_fichier_sortie()
    concat_cpp_headers(dossier, sortie)
