#/bin/bash

echo "========================clang-format-diff for $1========================"
diff <(cat $1) <(clang-format $1) --color
