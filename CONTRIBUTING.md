# Contributing

## Natural Workflow

- Set up development enviroment.
- Checkout new branch.
- Program
- Compile and deploy code
```sh
pio run # Compiles
pio run -t upload # Deploys
```
- Run Tests
```sh
pio test
```
- Finally push changes to repository
  ```sh
  git add -A
  git commit -m "feat(<package>): <description>"
  git push # git push -u origin <branch_name> if it is a new branch
  ```
  Note that the commit message should more or less follow the [Conventional Commits norms](https://www.conventionalcommits.org/en/v1.0.0-beta.4/)

## Notes on compilation, testing and platformio

When you perform any pio actions, you are performing them on all default environments. If you want to run tests on a specific environment:
```sh
pio test --environment [environment_name]
```

or compile and deploy...
```sh
pio run -t --environment [environment_name]
```
