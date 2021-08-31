[@deriving yojson]
type t = {
  difficulty: int,
  mutable unconfirmed_transactions: list(string),
  mutable chain: list(Block.t),
};

let add_new_transaction = (t, transaction) =>
  t.unconfirmed_transactions = [transaction, ...t.unconfirmed_transactions];

let create_genesis_block = () => {
  let time = Unix.gettimeofday();
  Block.create(0, [], time, "0");
};

let create = () => {
  let gb = create_genesis_block();
  gb.hash = Block.compute_hash(gb);
  {difficulty: 2, unconfirmed_transactions: [], chain: [gb]};
};

let last_block = t => List.hd(t.chain);

let proof_of_work = (difficulty: int, b: Block.t) => {
  b.nonce = 0;
  let computed_hash = ref(Block.compute_hash(b));
  let prefix = String.make(difficulty, '0');
  while (String.sub(computed_hash^, 0, difficulty) != prefix) {
    b.nonce = b.nonce + 1;
    computed_hash := Block.compute_hash(b);
  };
  computed_hash^;
};

let is_valid_proof = (t: t, b: Block.t, block_hash: string) =>
  String.sub(block_hash, 0, t.difficulty) == String.make(t.difficulty, '0')
  && block_hash == Block.compute_hash(b);

let add_block = (t: t, b: Block.t, proof: string) => {
  let l = last_block(t);
  let previous_hash = l.previous_hash;
  if (previous_hash !== b.previous_hash) {
    false;
  } else if (!is_valid_proof(t, b, proof)) {
    false;
  } else {
    b.hash = proof;
    t.chain = [b, ...t.chain];
    true;
  };
};

let mine = t =>
  if (List.length(t.unconfirmed_transactions) == 0) {
    false;
  } else {
    let last_b = last_block(t);
    let new_b =
      Block.create(
        last_b.index + 1,
        t.unconfirmed_transactions,
        Unix.gettimeofday(),
        last_b.previous_hash,
      );
    let proof = proof_of_work(t.difficulty, new_b);
    let _ = add_block(t, new_b, proof);
    t.unconfirmed_transactions = [];
    true;
  };
