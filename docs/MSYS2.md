# MSYS2 + UCRT64 Setup

Follow the official installer instructions at <https://www.msys2.org/>. Use the default path (`C:\msys64`). MSYS2 ships multiple environments; we rely on the base `MSYS` layer plus the `UCRT64` environment described here: <https://www.msys2.org/docs/environments/>.

## Base install

1. Open the **MSYS** terminal and update:

   ```bash
   pacman -Syu
   ```

2. Close when prompted, reopen MSYS, and run:

   ```bash
   pacman -Syu
   pacman -S base-devel git vim zsh
   ```

## Windows Terminal profiles

Add two profiles so you can choose between the build environment (UCRT64) and the base MSYS shell when needed:

```json
{
    "guid": "{17da3cac-b318-431e-8a3e-7fcdefe6d114}",
    "name": "UCRT64 / MSYS2",
    "commandline": "C:/msys64/msys2_shell.cmd -defterm -here -no-start -ucrt64 -shell zsh",
    "startingDirectory": "C:/msys64/home/%USERNAME%",
    "icon": "C:/msys64/ucrt64.ico",
    "hidden": false
},
{
    "guid": "{71160544-14d8-4194-af25-d05feeac7233}",
    "name": "MSYS / MSYS2",
    "commandline": "C:/msys64/msys2_shell.cmd -defterm -here -no-start -msys",
    "startingDirectory": "C:/msys64/home/%USERNAME%",
    "icon": "C:/msys64/msys2.ico",
    "hidden": false
}
```

You’ll rarely need the MSYS profile after setup, but it’s handy to keep around.

## UCRT64 environment

Launch the **UCRT64 / MSYS2** profile for day-to-day development.

- Optional: install [oh-my-zsh](https://ohmyz.sh/) or your preferred shell tweaks.
- Install toolchains and build utilities:

  ```bash
  pacman -Syu
  pacman -S \
      mingw-w64-ucrt-x86_64-toolchain \
      mingw-w64-ucrt-x86_64-make \
      mingw-w64-ucrt-x86_64-pkgconf \
      mingw-w64-ucrt-x86_64-cmake
  ```

## Git + SSH

Generate a keypair (adjust the email/comment) and configure git:

```bash
ssh-keygen -t ed25519 -C "your@email"
ssh-add ~/.ssh/id_ed25519

git config --global user.name "Your Name"
git config --global user.email "your@email"
git config --global core.sshCommand "ssh"
git config --global gpg.format ssh
git config --global user.signingkey ~/.ssh/id_ed25519.pub
git config --global commit.gpgsign true
```

## Shell profile (`~/.zshrc`)

Add environment variables and helpers so builds find the right tools and vcpkg triplets:

```bash
# Paths for VS Code (adjust if you use stable/insiders only)
export PATH="$PATH:/c/Users/$USERNAME/AppData/Local/Programs/Microsoft VS Code Insiders/bin"
export PATH="$PATH:/c/Users/$USERNAME/AppData/Local/Programs/Microsoft VS Code/bin"

# vcpkg
export VCPKG_ROOT=/c/tools/vcpkg
export VCPKG_DEFAULT_TRIPLET=x64-mingw-dynamic
export VCPKG_DEFAULT_HOST_TRIPLET=x64-mingw-dynamic

# Shortcuts
alias vi='vim'

# SSH agent
export SSH_AUTH_SOCK=/tmp/ssh-agent.sock
ssh_agent_is_running() { ssh-add -l >/dev/null 2>&1; }
if ! ssh_agent_is_running; then
  [ -S "$SSH_AUTH_SOCK" ] && rm -f "$SSH_AUTH_SOCK"
  eval "$(ssh-agent -a $SSH_AUTH_SOCK -s)" >/dev/null
  ssh-add ~/.ssh/id_ed25519 </dev/tty
fi
```

At this point the UCRT64 shell has the compilers, CMake, vcpkg defaults, and SSH credentials you need to work on Steeplejack.
