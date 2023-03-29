(*
 * Here is a sketch of the big-step semantics for the kernel:
 *
 * - bigstep (P, M, H, C, S) (P', M', H', C', S')
 *   - compute H (p, M) (p', M')
 *     /\ P' = {p'} UNION (P/p')
 *     /\ C = C /\ S = S' /\ H = H'
 *   - switch (P, H, S) (P, H', S) 
 *     /\ M = M' /\ C = C'
 *   - syscall (p, H, C) (p', H', C')
 *     /\ P' = {p'} UNION (P/p') 
 *     /\ S = S' /\ M = M'
 *
 * - P, P': Set of processes.
 * - H, H': Hardware context (e.g., PMP).
 * - S, S': Scheduling context.
 * - C, C': Capability sets.
 * - compute H (p, M) (p', M'): Process's compute step.
 * - switch (P, H, S) (P', H', S'): Schedule context switch.
 * - syscall (p, H, C) (p', H', C'): System call.
 *)


(* Capability defintions *)
(*
* - CapRoot
*   - Just a root capability.
* - CapTime hartid range alloc
*   - range: memory controlled by this capability.
*   - free: memory not allocated to children.
* - CapMemory range alloc l r w x
*   - range: memory controlled by this capability.
*   - free: memory not allocated to children.
*   - l: lock, set if there may be a child PMP slice.
*   - rwx: read write execute permissions.
* - CapPMP addr r w x
*   - range: PMP address range
*   - rwx: read write execute permissions.
* - CapChannel range alloc
*   - range: channels controlled by this capability.
*   - free: channels not allocated to other children.
* - CapSocket channel tag
*   - channel: communication channel.
*   - tag: if 0 then receiver, otherwise identifies sender.
*)
Datatype:
  cap_t = CapRoot
        | CapTime num (num set) (num set)
        | CapMemory (num set) (num set) bool bool bool bool
        | CapPMP (num set) bool bool bool
        | CapChannel (num set) (num set)
        | CapSocket num num
End

Datatype:
  cnode_t = CNode num capability_t (cnode_t list)
End

(* Check if the capability is well-formed, mainly, the allocated
* part of slices should be contained in the ranges. *)
Definition cap_is_wf_def:
cap_is_wf CapRoot = T
/\
cap_is_wf (CapTime hartid range free) = free SUBSET range
/\
cap_is_wf (CapMemory range free l r w x) = free SUBSET range
/\
cap_is_wf (CapPMP range r w x) = T
/\
cap_is_wf (CapChannel range free) = free SUBSET range
/\
cap_is_wf (CapSocket channel tag) = T
End

Definition cap_is_child_def:
  (
    cap_is_child (CapRoot) _ = False
  ) /\ (
    cap_is_child (CapMemory range free l r w x)
                 (CapMemory range' free' l' r' w' x')
                 = (range' SUBSET range /\ r ==> r' /\ w ==> w' /\ x ==> x')
  ) /\ (
    cap_is_child (CapMemory range free l r w x)
                 (CapPMP range' r' w' x')
                 = (range' SUBSET range /\ r ==> r' /\ w ==> w' /\ x ==> x')
  ) /\ (
    cap_is_child (CapTime hartid range free)
                 (CapTime hartid' range' free')
                 = (hartid = hartid' /\ range' SUBSET range)
  ) /\ (
    cap_is_child (CapChannel range free)
                 (CapChannel range' free')
                 = (range' SUBSET range)
  ) /\ (
    cap_is_child (CapChannel range free)
                 (CapSocket channel' tag')
                 = (channel IN range)
  )
End

val _ = Hol_reln `
 (compute H (p, M) (p', M') /\
 P' = {p'} UNION (P DIFF p)
 ==> bigstep (P, M, H, C, S) (P', M', H, C, S))
 /\
 (switch (P, H, M) (P',H' M') /\
 ==> bigstep (P, M, H, C, S) (P', M, H', C, S'))
 /\
 (syscall (p, H, C) (p',H' C') /\
 P' = {p'} UNION (P DIFF p)
 ==> bigstep (P, M, H, C, S) (P', M, H', C', S))
`;
