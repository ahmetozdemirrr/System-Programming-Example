# SIMPLE FILE SYSTEM

	This C program demonstrates basic file operations. It opens a file named "example.txt" (creates if not exists), writes predefined content, reads the content, prompts the user for input, appends the input to the file, and displays the final content.

## Usage

1. **Compiling and Running:**

    ```bash
    gcc filesys.c -o filesys
    ./filesys
    ```
    or 

    ```bash
    make
    ```

## Code Explanation

- The program opens a file using `open()` with appropriate flags and modes.

- It writes predefined content to the file using `write()`.

- The program calculates the number of lines in the file by reading and counting newline characters.

- After displaying the initial content, it prompts the user for input and appends it to the file using `write()`.

- It recalculates the number of lines and displays the final content.

## Notes

- The program uses low-level file I/O functions `(open(), read(), write())` for demonstration purposes.

- File access modes `(O_CREAT, O_RDWR)` and permissions `(S_IRUSR, S_IWUSR)` are explained in comments.

- The usage of `lseek()` to move the file pointer is demonstrated.

- Standard input/output file descriptors `(STDIN_FILENO, STDOUT_FILENO)` are used with `write()` for output.

## Sample Interaction

```bash

-- Before input --
Line count: 7
Hello world!
Hello world!
Hello world!
Hello world!
Hello world!
Hello world!
Hello world!

Please enter an input : I also love programming.

-- After input --
Line count: 8
Hello world!
Hello world!
Hello world!
Hello world!
Hello world!
Hello world!
Hello world!
I also love programming.
```

Feel free to reach out if you have any questions or need further clarification. Happy coding! 🚀