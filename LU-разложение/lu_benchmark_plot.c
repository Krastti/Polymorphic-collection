//
// Created by Krastti on 04.03.2026.
//

#include <stdio.h>
#include "lu_benchmark.h"

void lu_benchmark_save_html(const char* filename, size_t* sizes, double* times_doolittle, double* times_crout,int* success_d,int* success_c,size_t num_sizes)
{
    FILE* f = fopen(filename, "w");
    if (!f) {
        fprintf(stderr, "Ошибка: не удалось создать %s\n", filename);
        return;
    }

    fprintf(f, "<!DOCTYPE html>\n<html lang=\"ru\">\n<head>\n");
    fprintf(f, "<meta charset=\"UTF-8\">\n");
    fprintf(f, "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n");
    fprintf(f, "<title>Бенчмарк LU-разложения</title>\n");
    fprintf(f, "<script src=\"https://cdn.jsdelivr.net/npm/chart.js\"></script>\n");
    fprintf(f, "<style>\n");
    fprintf(f, "  body { font-family: Arial, sans-serif; margin: 40px; background: #f5f5f5; }\n");
    fprintf(f, "  .container { max-width: 1200px; margin: 0 auto; background: white; ");
    fprintf(f, "padding: 30px; border-radius: 10px; box-shadow: 0 2px 10px rgba(0,0,0,0.1); }\n");
    fprintf(f, "  h1 { color: #333; text-align: center; }\n");
    fprintf(f, "  .stats { display: grid; grid-template-columns: repeat(auto-fit, minmax(200px, 1fr)); ");
    fprintf(f, "gap: 20px; margin: 30px 0; }\n");
    fprintf(f, "  .stat-card { background: #f8f9fa; padding: 20px; border-radius: 8px; ");
    fprintf(f, "text-align: center; }\n");
    fprintf(f, "  .stat-value { font-size: 24px; font-weight: bold; color: #2E86AB; }\n");
    fprintf(f, "  .stat-label { color: #666; margin-top: 5px; }\n");
    fprintf(f, "  table { width: 100%%; border-collapse: collapse; margin-top: 30px; }\n");
    fprintf(f, "  th, td { padding: 12px; text-align: center; border-bottom: 1px solid #ddd; }\n");
    fprintf(f, "  th { background: #2E86AB; color: white; }\n");
    fprintf(f, "  tr:hover { background: #f5f5f5; }\n");
    fprintf(f, "  .success { color: green; } .error { color: red; }\n");
    fprintf(f, "</style>\n</head>\n<body>\n");
    fprintf(f, "<div class=\"container\">\n");
    fprintf(f, "<h1>📊 Бенчмарк LU-разложения: Doolittle vs Crout</h1>\n");

    // Статистика
    fprintf(f, "<div class=\"stats\">\n");
    fprintf(f, "  <div class=\"stat-card\">\n");
    fprintf(f, "    <div class=\"stat-value\">%zu</div>\n", num_sizes);
    fprintf(f, "    <div class=\"stat-label\">Точек данных</div>\n");
    fprintf(f, "  </div>\n");
    fprintf(f, "  <div class=\"stat-card\">\n");
    fprintf(f, "    <div class=\"stat-value\">%zux%zu</div>\n", sizes[0], sizes[num_sizes-1]);
    fprintf(f, "    <div class=\"stat-label\">Диапазон размеров</div>\n");
    fprintf(f, "  </div>\n");
    fprintf(f, "</div>\n");

    // График
    fprintf(f, "<canvas id=\"benchmarkChart\"></canvas>\n");

    // Таблица
    fprintf(f, "<h2>📋 Таблица результатов</h2>\n");
    fprintf(f, "<table>\n");
    fprintf(f, "  <thead>\n");
    fprintf(f, "    <tr>\n");
    fprintf(f, "      <th>Размер</th>\n");
    fprintf(f, "      <th>Doolittle (мс)</th>\n");
    fprintf(f, "      <th>Crout (мс)</th>\n");
    fprintf(f, "      <th>Speedup</th>\n");
    fprintf(f, "      <th>Статус</th>\n");
    fprintf(f, "    </tr>\n");
    fprintf(f, "  </thead>\n");
    fprintf(f, "  <tbody>\n");

    for (size_t i = 0; i < num_sizes; i++) {
        double speedup = (times_crout[i] > 0.001) ? times_doolittle[i] / times_crout[i] : 1.0;
        const char* status = (success_d[i] && success_c[i]) ?
                             "<span class=\"success\">✓ Успешно</span>" :
                             "<span class=\"error\">✗ Ошибка</span>";

        fprintf(f, "    <tr>\n");
        fprintf(f, "      <td>%zux%zu</td>\n", sizes[i], sizes[i]);
        fprintf(f, "      <td>%.4f</td>\n", times_doolittle[i]);
        fprintf(f, "      <td>%.4f</td>\n", times_crout[i]);
        fprintf(f, "      <td>%.2f</td>\n", speedup);
        fprintf(f, "      <td>%s</td>\n", status);
        fprintf(f, "    </tr>\n");
    }

    fprintf(f, "  </tbody>\n</table>\n");
    fprintf(f, "</div>\n");

    // JavaScript для графика
    fprintf(f, "<script>\n");
    fprintf(f, "const ctx = document.getElementById('benchmarkChart').getContext('2d');\n");
    fprintf(f, "new Chart(ctx, {\n");
    fprintf(f, "  type: 'line',\n");
    fprintf(f, "  data: {\n");
    fprintf(f, "    labels: [");
    for (size_t i = 0; i < num_sizes; i++) {
        fprintf(f, "'%zux%zu'%s", sizes[i], sizes[i], (i < num_sizes-1) ? ", " : "");
    }
    fprintf(f, "],\n");
    fprintf(f, "    datasets: [{\n");
    fprintf(f, "      label: 'Doolittle (мс)',\n");
    fprintf(f, "      data: [");
    for (size_t i = 0; i < num_sizes; i++) {
        fprintf(f, "%.4f%s", times_doolittle[i], (i < num_sizes-1) ? ", " : "");
    }
    fprintf(f, "],\n");
    fprintf(f, "      borderColor: 'rgb(46, 134, 171)',\n");
    fprintf(f, "      backgroundColor: 'rgba(46, 134, 171, 0.1)',\n");
    fprintf(f, "      tension: 0.1,\n");
    fprintf(f, "      fill: true\n");
    fprintf(f, "    }, {\n");
    fprintf(f, "      label: 'Crout (мс)',\n");
    fprintf(f, "      data: [");
    for (size_t i = 0; i < num_sizes; i++) {
        fprintf(f, "%.4f%s", times_crout[i], (i < num_sizes-1) ? ", " : "");
    }
    fprintf(f, "],\n");
    fprintf(f, "      borderColor: 'rgb(162, 59, 114)',\n");
    fprintf(f, "      backgroundColor: 'rgba(162, 59, 114, 0.1)',\n");
    fprintf(f, "      tension: 0.1,\n");
    fprintf(f, "      fill: true\n");
    fprintf(f, "    }]\n");
    fprintf(f, "  },\n");
    fprintf(f, "  options: {\n");
    fprintf(f, "    responsive: true,\n");
    fprintf(f, "    plugins: {\n");
    fprintf(f, "      title: { display: true, text: 'Время выполнения от размера матрицы', font: { size: 16 } },\n");
    fprintf(f, "      legend: { display: true, position: 'top' }\n");
    fprintf(f, "    },\n");
    fprintf(f, "    scales: {\n");
    fprintf(f, "      y: { \n");
    fprintf(f, "        beginAtZero: true, \n");
    fprintf(f, "        title: { display: true, text: 'Время (мс)' }\n");
    fprintf(f, "      },\n");
    fprintf(f, "      x: { \n");
    fprintf(f, "        title: { display: true, text: 'Размер матрицы' }\n");
    fprintf(f, "      }\n");
    fprintf(f, "    }\n");
    fprintf(f, "  }\n");
    fprintf(f, "});\n");
    fprintf(f, "</script>\n</body>\n</html>\n");

    fclose(f);
    printf("✓ HTML с графиком сохранён в %s\n", filename);
    printf("  Откройте файл в браузере для просмотра\n");
}