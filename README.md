# Recommendation Engine As a Redis Module

Redis Too is a Redis module that implements a Jaccardian similarity and memory-based collaborative filtering recommendation engine.

## Usage

To build and load:

``` sh
make
redis-server --loadmodule ./too.so
```

To add likes and generate recommendations:

```
# Add likes
redis> TOO.LIKE movies "The Shawshank Redemption" Sonic
redis> TOO.LIKE movies "The Godfather" Sonic
redis> TOO.LIKE movies "The Dark Knight" Sonic
redis> TOO.LIKE movies "Pulp Fiction" Sonic

redis> TOO.LIKE movies "The Godfather" Mario
redis> TOO.LIKE movies "The Dark Knight" Mario
redis> TOO.LIKE movies "The Shawshank Redemption" Mario
redis> TOO.LIKE movies "The Prestige" Mario
redis> TOO.LIKE movies "The Matrix" Mario

redis> TOO.LIKE movies "The Godfather" Peach
redis> TOO.LIKE movies "Inception" Peach
redis> TOO.LIKE movies "Fight Club" Peach
redis> TOO.LIKE movies "WALL·E" Peach
redis> TOO.LIKE movies "Princess Mononoke" Peach

redis> TOO.LIKE movies "The Prestige" Luigi
redis> TOO.LIKE movies "The Dark Knight" Luigi

# Refresh recommendations
redis> TOO.REFRESH movies Sonic
redis> TOO.REFRESH movies Mario
redis> TOO.REFRESH movies Peach
redis> TOO.REFRESH movies Luigi

# Get recommendations
redis> TOO.SUGGEST movies Luigi
1) "The Shawshank Redemption"
2) "The Matrix"
3) "Pulp Fiction"
4) "The Godfather"
```

## Commands

- `TOO.LIKE class item user`: Adds `item` of type `class` to the user's list of likes.
- `TOO.UNLIKE class item user`: Removes `item` of type `class` to the user's list of likes.
- `TOO.DISLIKE class item user`: Adds `item` of type `class` to user's list of dislikes.
- `TOO.UNDISLIKE class item user`: Removes `item` of type `class` to user's list of dislikes.
- `TOO.REFRESH class user`: Refreshes list of suggestions for `user` of type `class`.
- `TOO.SUGGEST class user`: Returns list of suggestions for `user` of type `class`.

## Contributing

Contributions are welcome.

## To-do

- [ ] Add tests.
- [ ] Ignore items added to the `ignores` list.
- [ ] Add an option to auto-refresh suggestions.
- [ ] Add an option to limit the number of similar users to check when refreshing suggestions.

## License

Redis Too is available under the [BSD (3-Clause) License](LICENSE).
