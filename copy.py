import os

def demander_dossier():
    while True:
        dossier = input("Entrez le chemin du dossier à explorer : ").strip()
        if os.path.isdir(dossier):
            return dossier
        else:
            print("Chemin invalide ou ce n'est pas un dossier. Veuillez réessayer.")

def demander_fichier_output():
    return input("Entrez le nom du fichier de sortie (ex: resultat.txt) : ").strip()

def concat_files_recursively(input_dir, output_file):
    with open(output_file, 'w', encoding='utf-8') as outfile:
        for root, dirs, files in os.walk(input_dir):
            for filename in files:
                file_path = os.path.join(root, filename)
                outfile.write(f"\n--- {file_path} ---\n")
                try:
                    with open(file_path, 'r', encoding='utf-8') as infile:
                        outfile.write(infile.read())
                except Exception as e:
                    outfile.write(f"[Erreur de lecture: {e}]\n")

if __name__ == "__main__":
    dossier_a_explorer = demander_dossier()
    fichier_de_sortie = demander_fichier_output()
    concat_files_recursively(dossier_a_explorer, fichier_de_sortie)
    print(f"Fichier '{fichier_de_sortie}' généré avec succès.")
