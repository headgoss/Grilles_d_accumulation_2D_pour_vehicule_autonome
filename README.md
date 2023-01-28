# Grilles_d_accumulation_2D_pour_vehicule_autonome
Ce projet s’intéresse à la partie « Perception de l’environnement » dont la mission principale est
d’acquérir les données des différents capteurs (caméras, lidars, etc...), de les traiter et de les représenter pour pouvoir ensuite corriger la trajectoire (éviter les obstacles par exemple). Parmi les méthodes utilisées dans la littérature scientifique, les grilles d’occupation prennent une place primordiale. Il s’agit de découper les mesures du liddar en une grille 2D afin de déterminer si chacune des cellules est occupée. Tout récemment, l’IRIMAS s’intéresse à l’amélioration de ces grilles en les découpant en 3 zones :
- Zone 1 : Proximité immédiate du véhicule, freinage d’urgence requis (opérationnel)
- Zone 2 : Scène proche du véhicule, ralentissement et négociation de trajectoire
(tactique)
- Zone 3 : Horizon du véhicule, trajectoire à long terme (stratégique)
