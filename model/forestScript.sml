open HolKernel Parse boolLib bossLib set_relationTheory;

val _ = new_theory "forest"

Definition injective_def:
  injective r = (!x y z. (x,z) IN r /\ (y,z) IN r ==> x = y)
End

Definition forest_def:
 forest r = (acyclic r /\ injective r)
End

Definition children_def:
  children r x = {y | (x,y) IN r}
End

Definition parent_def:
  parent r x = {y | (y,x) IN r}
End

Definition descendants_def:
  descendants r x = {y | (x,y) IN (tc r)}
End

(* Delete node x 
 * - Remove all edges involving x
 * - Add children of x to parent(x)
 *)
Definition delete_def:
  delete r x = {(z,y) | (z,y) IN r /\ x <> z /\ x <> y } 
               UNION {(z,y) | (z,x) IN r /\ (x,y) IN r}
End


(* In a forest, each node has at most one parent. *)
Theorem forest_CARD_parent_def:
!r x.
  forest r ==> (CARD (parent r x) <= 1)
Proof
  cheat
QED

(* When we delete a node, the parent of the node inherits the children. *)
Theorem delete_inherit_thm:
!r x.
  {(y,z) | y IN (parent r x) /\ z IN (children r x)} SUBSET (delete r x)
Proof
  cheat
QED

(* Children are descendants. *)
Theorem children_SUBSET_descendants_thm:
!r x.
  (children r x) SUBSET (descendants r x)
Proof
  cheat
QED

Theorem children_SUBSET_range:
!r x.
  (children r x) SUBSET (range r)
Proof
  cheat
QED

Theorem parent_SUBSET_domain:
!r x.
  (parent r x) SUBSET (domain r)
Proof
  cheat
QED

val _ = export_theory ();
