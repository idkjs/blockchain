[@deriving yojson]
type t = {
  index: int,
  transactions: list(string),
  timestamp: float,
  mutable hash: string,
  previous_hash: string,
  mutable nonce: int,
};

let compute_hash: t => string;

let create: (int, list(string), float, string) => t;
