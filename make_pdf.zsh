#!/bin/bash

output_tex="combined_output.tex"
output_pdf="output.pdf"
temp_dir="code_temp"
mkdir -p "$temp_dir"

# Start LaTeX file
cat > "$output_tex" <<EOF
\\documentclass[11pt]{article}
\\usepackage[a4paper,margin=1in]{geometry}
\\usepackage{fancyhdr}
\\usepackage{titlesec}
\\usepackage{color}
\\definecolor{gray}{rgb}{0.5,0.5,0.5}
\\usepackage{listings}
\\usepackage{inconsolata}
\\pagestyle{fancy}
\\fancyhf{}
\\renewcommand{\\headrulewidth}{0.4pt}
\\titleformat{\\section}{\\Large\\bfseries}{\\thesection}{1em}{}

\\lstdefinestyle{cppstyle}{
  language=C++,
  basicstyle=\\ttfamily\\small,
  keywordstyle=\\color{blue}\\bfseries,
  commentstyle=\\color{gray}\\itshape,
  stringstyle=\\color{red},
  showstringspaces=false,
  breaklines=true,
  tabsize=2
}

\\begin{document}
EOF

# Function to add a file to the LaTeX doc
add_file() {
    local filepath="$1"
    local filename=$(basename "$filepath")

    echo "\\section*{$filename}" >> "$output_tex"
    echo "\\lstinputlisting[style=cppstyle]{${temp_dir}/$filename}" >> "$output_tex"
    echo "\\newpage" >> "$output_tex"

    cp "$filepath" "${temp_dir}/$filename"
}

# Add main.cpp first if it exists
[[ -f "main.cpp" ]] && add_file "main.cpp"

# Process cpp/hpp pairs
for cpp_file in $(find src -type f -name '*.cpp' ! -name 'main.cpp' | sort); do
    base=$(basename "$cpp_file" .cpp)

    # Try to find matching header
    if [[ -f "include/$base.h" ]]; then
        header_file="include/$base.h"
    elif [[ -f "include/$base.hpp" ]]; then
        header_file="include/$base.hpp"
    else
        header_file=""
    fi

    add_file "$cpp_file"
    [[ -n "$header_file" ]] && add_file "$header_file"
done

# Close LaTeX
echo "\\end{document}" >> "$output_tex"

# Compile PDF
pdflatex "$output_tex" > /dev/null
mv combined_output.pdf "$output_pdf"
rm -rf "$temp_dir" *.aux *.log combined_output.tex
echo "✅ PDF generated as: $output_pdf"
