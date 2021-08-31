[@deriving yojson]
type t = {
  difficulty: int,
  mutable unconfirmed_transactions: list(string),
  mutable chain: list(Block.t),
};

let create: unit => t;

let last_block: t => Block.t;

let proof_of_work: (int, Block.t) => string;

let add_block: (t, Block.t, string) => bool;

let add_new_transaction: (t, string) => unit;

let mine: t => bool;
