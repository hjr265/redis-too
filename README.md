# Recommendation Engine as Redis Module

Redis Too is a Redis module that implements a Jaccardian similarity and memory-based collaborative filtering recommendation engine.

## Usage

```
# Add likes
> TOO.LIKE movies "The Shawshank Redemption" Sonic
> TOO.LIKE movies "The Godfather" Sonic
> TOO.LIKE movies "The Dark Knight" Sonic
> TOO.LIKE movies "Pulp Fiction" Sonic

> TOO.LIKE movies "The Godfather" Mario
> TOO.LIKE movies "The Dark Knight" Mario
> TOO.LIKE movies "The Shawshank Redemption" Mario
> TOO.LIKE movies "The Prestige" Mario
> TOO.LIKE movies "The Matrix" Mario

> TOO.LIKE movies "The Godfather" Peach
> TOO.LIKE movies "Inception" Peach
> TOO.LIKE movies "Fight Club" Peach
> TOO.LIKE movies "WALL·E" Peach
> TOO.LIKE movies "Princess Mononoke" Peach

> TOO.LIKE movies "The Prestige" Luigi
> TOO.LIKE movies "The Dark Knight" Luigi

# Refresh recommendations
> TOO.REFRESH movies Sonic
> TOO.REFRESH movies Mario
> TOO.REFRESH movies Peach
> TOO.REFRESH movies Luigi

# Get recommendations
> TOO.SUGGEST movies Luigi
1) "The Shawshank Redemption"
2) "The Matrix"
3) "Pulp Fiction"
4) "The Godfather"
```

## Commands

- `TOO.LIKE class item user`: Adds `item` of type `class` to the user's list of likes.
- `TOO.DISLIKE class item user`: Adds `item` of type `class` to user's list of dislikes.
- `TOO.REFRESH class user`: Refreshes list of suggestions for `user` of type `class`.
- `TOO.SUGGEST class user`: Returns list of suggestions for `user` of type `class`.

## Contributing

Contributions are welcome.

## License

Redis Too is available under the [BSD (3-Clause) License](LICENSE).
