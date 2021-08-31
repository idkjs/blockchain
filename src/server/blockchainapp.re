open Opium;

[@deriving yojson]
type chain_t = {
  length: int,
  chain: list(Block.t),
};

[@deriving yojson]
type transaction_t = {
  author: string,
  content: string,
};

let blockchain = Blockchain.create();

let new_transaction = req => {
  open Lwt.Syntax;
  let* body = Body.to_string(req.Request.body);
  let _ = Logs_lwt.app(m => m("Received: %s", body));
  let json = Yojson.Safe.from_string(body);
  let ret = transaction_t_of_yojson(json);
  switch (ret) {
  | Ok(trans_value) =>
    Blockchain.add_new_transaction(blockchain, trans_value.content);
    Response.of_json(transaction_t_to_yojson(trans_value)) |> Lwt.return;
  | Error(err_str) =>
    let status = Status.of_code(404);
    Response.of_plain_text(~status, err_str) |> Lwt.return;
  };
};

let get_chain = _ => {
  let chain = {
    length: List.length(blockchain.chain),
    chain: blockchain.chain,
  };
  let blockchain_str = chain_t_to_yojson(chain);
  let _ = Logs_lwt.app(m => m("chain request"));
  blockchain_str |> Response.of_json |> Lwt.return;
};

let mine = _ => {
  let result = Blockchain.mine(blockchain);
  let result_str =
    if (!result) {
      "No transaction to mine";
    } else {
      "Block is mined";
    };
  result_str |> Response.of_plain_text |> Lwt.return;
};

let _ = {
  Logs.set_level(Some(Logs.Info));
  Logs.set_reporter(Logs.format_reporter());
  Logs.app(m => m("Blockchain app started..."));
  let static = Middleware.static_unix(~local_path=".", ~uri_prefix="/", ());

  App.empty
  |> App.middleware(static)
  |> App.get("/chain", get_chain)
  |> App.post("/new_transaction", new_transaction)
  |> App.get("/mine", mine)
  |> App.run_command;
};
