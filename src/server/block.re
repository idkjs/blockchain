[@deriving yojson]
type t = {
  index: int,
  transactions: list(string),
  timestamp: float,
  mutable hash: string,
  previous_hash: string,
  mutable nonce: int,
};

[@deriving yojson]
type hashtable_t = {
  index: int,
  transactions: list(string),
  timestamp: float,
  previous_hash: string,
  nonce: int,
};

/* compute the hash from all fields except the hash field
   or assuming the hash field is empty */
let compute_hash = (t: t) => {
  let h = {
    index: t.index,
    transactions: t.transactions,
    timestamp: t.timestamp,
    previous_hash: t.previous_hash,
    nonce: t.nonce,
  };
  let block_str = Yojson.Safe.to_string(hashtable_t_to_yojson(h));
  Digestif.SHA256.to_hex(Digestif.SHA256.digest_string(block_str));
};

let create = (index, transactions, timestamp, previous_hash) => {
  let b = {
    index,
    transactions,
    timestamp,
    hash: "0",
    previous_hash,
    nonce: 0,
  };
  let hash = compute_hash(b);

  b.hash = hash;
  b;
};
